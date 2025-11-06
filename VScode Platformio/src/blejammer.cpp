/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#include "../include/blejammer.h"
#include "../include/sleep_manager.h"
#include "../include/pindefs.h"
#include <esp_bt_main.h>

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <esp_wifi.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

#define CE_PIN_1 RADIO_CE_PIN_1
#define CSN_PIN_1 RADIO_CSN_PIN_1
#define CE_PIN_2 RADIO_CE_PIN_2
#define CSN_PIN_2 RADIO_CSN_PIN_2
#define CE_PIN_3 RADIO_CE_PIN_3
#define CSN_PIN_3 RADIO_CSN_PIN_3

RF24 radio1(CE_PIN_1, CSN_PIN_1, 16000000);
RF24 radio2(CE_PIN_2, CSN_PIN_2, 16000000);
RF24 radio3(CE_PIN_3, CSN_PIN_3, 16000000);

enum BleJammerMode { JAM_MENU, JAM_BLE, JAM_BLUETOOTH, JAM_ALL };
static BleJammerMode jammerMode = JAM_MENU;
static int menuSelection = 0;
static unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

const byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0,  1,  2,  4, 6,
                                   8,  22, 24, 26, 28, 30, 74, 76, 78, 80};
const byte ble_channels[] = {2, 26, 80};

void configureRadio(RF24 &radio, byte initialChannel) {
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.printPrettyDetails();
  
  radio.startConstCarrier(RF24_PA_MAX, initialChannel);
}

void initializeRadiosForBLE() {
  SPI.begin();
  delay(100);
  
  if (radio1.begin()) configureRadio(radio1, 2);
  if (radio2.begin()) configureRadio(radio2, 26);
  if (radio3.begin()) configureRadio(radio3, 80);
}

void powerDownRadios() {
  radio1.powerDown();
  radio2.powerDown();
  radio3.powerDown();
  delay(100);
}

static void drawJammerMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 10, "BLE Jammer Mode:");
  u8g2.drawStr(0, 22, menuSelection == 0 ? "> All" : "  All");
  u8g2.drawStr(0, 32, menuSelection == 1 ? "> Bluetooth" : "  Bluetooth");
  u8g2.drawStr(0, 42, menuSelection == 2 ? "> BLE" : "  BLE");
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 62, "U/D=Move R=Start SEL=Exit");
  u8g2.sendBuffer();
}

void blejammerSetup() {
  Serial.begin(115200);

  esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
  if (bt_state == ESP_BLUEDROID_STATUS_ENABLED) {
      esp_bluedroid_disable();
      delay(50);
  }
  if (bt_state != ESP_BLUEDROID_STATUS_UNINITIALIZED) {
      esp_bluedroid_deinit();
      delay(50);
  }
  
  if (btStarted()) {
      btStop();
      delay(50);
  }

  wifi_mode_t mode;
  if (esp_wifi_get_mode(&mode) == ESP_OK) {
      esp_wifi_stop();
      delay(50);
      esp_wifi_deinit();
      delay(100);
  }

  esp_netif_t* sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (sta_netif != NULL) {
      esp_netif_destroy(sta_netif);
  }

  esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
  if (ap_netif != NULL) {
      esp_netif_destroy(ap_netif);
  }

  pinMode(BUTTON_PIN_UP, INPUT_PULLUP);
  pinMode(BUTTON_PIN_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_PIN_LEFT, INPUT_PULLUP);

  jammerMode = JAM_MENU;
  menuSelection = 0;
  powerDownRadios();
  drawJammerMenu();
}

static void drawActiveJamming(const char* modeName) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, modeName);
  u8g2.drawStr(0, 28, "Status: Active");
  u8g2.setFont(u8g2_font_5x8_tr);
  char radioStatus[32];
  snprintf(radioStatus, sizeof(radioStatus), "R1:%s R2:%s R3:%s",
           radio1.isChipConnected() ? "ON" : "OFF",
           radio2.isChipConnected() ? "ON" : "OFF",
           radio3.isChipConnected() ? "ON" : "OFF");
  u8g2.drawStr(0, 42, radioStatus);
  u8g2.drawStr(0, 62, "L=Back SEL=Exit");
  u8g2.sendBuffer();
}

void blejammerLoop() {
  unsigned long now = millis();
  static BleJammerMode previousMode = JAM_MENU;

  bool up = digitalRead(BUTTON_PIN_UP) == LOW;
  bool down = digitalRead(BUTTON_PIN_DOWN) == LOW;
  bool left = digitalRead(BUTTON_PIN_LEFT) == LOW;
  bool right = digitalRead(BUTTON_PIN_RIGHT) == LOW;

  switch (jammerMode) {
    case JAM_MENU:
      drawJammerMenu();
      if (now - lastButtonPress > debounceDelay) {
        if (up) {
          menuSelection = (menuSelection - 1 + 3) % 3;
          lastButtonPress = now;
        } else if (down) {
          menuSelection = (menuSelection + 1) % 3;
          lastButtonPress = now;
        } else if (right) {
          if (menuSelection == 0) {
            jammerMode = JAM_ALL;
            initializeRadiosForBLE();
          } else if (menuSelection == 1) {
            jammerMode = JAM_BLUETOOTH;
            initializeRadiosForBLE();
          } else {
            jammerMode = JAM_BLE;
            initializeRadiosForBLE();
          }
          lastButtonPress = now;
        }
      }
      break;

    case JAM_BLE:
      if (jammerMode != previousMode) {
        drawActiveJamming("BLE Jamming");
        previousMode = jammerMode;
      }

      {
        int randomIndex1 = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
        int randomIndex2 = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
        int randomIndex3 = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
        
        int channel1 = ble_channels[randomIndex1];
        int channel2 = ble_channels[randomIndex2];
        int channel3 = ble_channels[randomIndex3];
        
        radio1.setChannel(channel1);
        radio2.setChannel(channel2);
        radio3.setChannel(channel3);
      }

      if (left && now - lastButtonPress > debounceDelay) {
        powerDownRadios();
        jammerMode = JAM_MENU;
        previousMode = JAM_BLE;
        lastButtonPress = now;
      }
      break;

    case JAM_BLUETOOTH:
      if (jammerMode != previousMode) {
        drawActiveJamming("Bluetooth Jamming");
        previousMode = jammerMode;
      }

      {
        int randomIndex1 = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
        int randomIndex2 = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
        int randomIndex3 = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
        
        int channel1 = bluetooth_channels[randomIndex1];
        int channel2 = bluetooth_channels[randomIndex2];
        int channel3 = bluetooth_channels[randomIndex3];
        
        radio1.setChannel(channel1);
        radio2.setChannel(channel2);
        radio3.setChannel(channel3);
      }

      if (left && now - lastButtonPress > debounceDelay) {
        powerDownRadios();
        jammerMode = JAM_MENU;
        previousMode = JAM_BLUETOOTH;
        lastButtonPress = now;
      }
      break;

    case JAM_ALL:
      if (jammerMode != previousMode) {
        drawActiveJamming("BT & BLE Jamming");
        previousMode = jammerMode;
      }

      {
        static bool useBLE = true;
        const byte* channelArray;
        int arraySize;
        
        if (useBLE) {
          channelArray = ble_channels;
          arraySize = sizeof(ble_channels) / sizeof(ble_channels[0]);
        } else {
          channelArray = bluetooth_channels;
          arraySize = sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]);
        }
        
        int randomIndex1 = random(0, arraySize);
        int randomIndex2 = random(0, arraySize);
        int randomIndex3 = random(0, arraySize);
        
        int channel1 = channelArray[randomIndex1];
        int channel2 = channelArray[randomIndex2];
        int channel3 = channelArray[randomIndex3];
        
        radio1.setChannel(channel1);
        radio2.setChannel(channel2);
        radio3.setChannel(channel3);
        
        useBLE = !useBLE;
      }

      if (left && now - lastButtonPress > debounceDelay) {
        powerDownRadios();
        jammerMode = JAM_MENU;
        previousMode = JAM_ALL;
        lastButtonPress = now;
      }
      break;
  }
}