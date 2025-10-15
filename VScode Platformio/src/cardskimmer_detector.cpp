/*
   ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________
*/

#include "../include/cardskimmer_detector.h"
#include "../include/sleep_manager.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

#define BTN_UP BUTTON_PIN_UP
#define BTN_DOWN BUTTON_PIN_DOWN
#define BTN_RIGHT BUTTON_PIN_RIGHT
#define BTN_BACK BUTTON_PIN_LEFT
#define BTN_CENTER BUTTON_PIN_CENTER

struct SkimmerDeviceData {
  char name[32];
  char address[18];
  int8_t rssi;
  unsigned long lastSeen;
};

static std::vector<SkimmerDeviceData> skimmerDevices;
const int MAX_DEVICES = 100;

const int KNOWN_SKIMMER_COUNT = 3;
const char* KNOWN_SKIMMERS[KNOWN_SKIMMER_COUNT] = {
  "HC-03",
  "HC-05",
  "HC-06"
};

int currentIndex = 0;
int listStartIndex = 0;
bool isDetailView = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceTime = 200;

static BLEScan *pBLEScan = nullptr;
static bool isScanning = false;
static unsigned long lastScanTime = 0;
const unsigned long SCAN_INTERVAL = 30000;
const unsigned long SCAN_DURATION = 5000;

static int skimmerCallbackCount = 0;
static unsigned long lastCallbackTime = 0;

bool isKnownSkimmer(const char* deviceName) {
  for (int i = 0; i < KNOWN_SKIMMER_COUNT; i++) {
    if (strcmp(deviceName, KNOWN_SKIMMERS[i]) == 0) {
      return true;
    }
  }
  return false;
}

class CardSkimmerCallback : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    skimmerCallbackCount++;

    unsigned long now = millis();
    if (now - lastCallbackTime < 50) {
      return;
    }
    lastCallbackTime = now;

    if (skimmerCallbackCount > 500 || skimmerDevices.size() >= MAX_DEVICES) {
      if (isScanning && pBLEScan) {
        pBLEScan->stop();
        isScanning = false;
      }
      return;
    }

    if (skimmerDevices.size() > 80 && skimmerCallbackCount % 2 != 0) return;

    if (!advertisedDevice.haveName()) return;

    std::string nameStd = advertisedDevice.getName();
    if (nameStd.length() == 0 || nameStd.length() >= 32) return;

    char deviceName[32];
    strncpy(deviceName, nameStd.c_str(), 31);
    deviceName[31] = '\0';

    if (!isKnownSkimmer(deviceName)) return;

    BLEAddress addr = advertisedDevice.getAddress();
    const char* addrCStr = addr.toString().c_str();

    char addrStr[18];
    strncpy(addrStr, addrCStr, 17);
    addrStr[17] = '\0';

    if (strlen(addrStr) < 12) return;

    int8_t deviceRSSI = advertisedDevice.getRSSI();

    for (int i = 0; i < skimmerDevices.size(); i++) {
      if (strcmp(skimmerDevices[i].address, addrStr) == 0) {
        skimmerDevices[i].rssi = deviceRSSI;
        skimmerDevices[i].lastSeen = millis();

        std::sort(skimmerDevices.begin(), skimmerDevices.end(),
                  [](const SkimmerDeviceData &a, const SkimmerDeviceData &b) {
                    return a.rssi > b.rssi;
                  });
        return;
      }
    }

    SkimmerDeviceData newDev = {};
    strncpy(newDev.address, addrStr, 17);
    newDev.address[17] = '\0';
    strncpy(newDev.name, deviceName, 31);
    newDev.name[31] = '\0';
    newDev.rssi = deviceRSSI;
    newDev.lastSeen = millis();

    skimmerDevices.push_back(newDev);

    std::sort(skimmerDevices.begin(), skimmerDevices.end(),
              [](const SkimmerDeviceData &a, const SkimmerDeviceData &b) {
                return a.rssi > b.rssi;
              });
  }
};

static CardSkimmerCallback skimmerCallbacks;

void cardskimmerDetectorSetup() {
  skimmerDevices.clear();
  skimmerDevices.reserve(MAX_DEVICES);
  currentIndex = listStartIndex = 0;
  isDetailView = false;
  lastButtonPress = 0;
  isScanning = false;
  skimmerCallbackCount = 0;
  lastCallbackTime = 0;

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Skimmer Detector");
  u8g2.drawStr(0, 20, "Initializing...");
  u8g2.sendBuffer();

  BLEDevice::init("SkimmerDetector");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(&skimmerCallbacks);
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(1000);
  pBLEScan->setWindow(200);

  pBLEScan->start(SCAN_DURATION / 1000, false);
  isScanning = true;
  lastScanTime = millis();

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_CENTER, INPUT_PULLUP);
}

void cardskimmerDetectorLoop() {
  unsigned long now = millis();

  if (now - lastScanTime > 10000) {
    skimmerCallbackCount = 0;
  }

  if (isScanning && now - lastScanTime > SCAN_DURATION) {
    pBLEScan->stop();
    isScanning = false;
    skimmerCallbackCount = 0;
    lastScanTime = now;
  }
  else if (!isScanning && now - lastScanTime > SCAN_INTERVAL) {
    if (skimmerDevices.size() >= MAX_DEVICES) {
      std::sort(skimmerDevices.begin(), skimmerDevices.end(),
                [](const SkimmerDeviceData &a, const SkimmerDeviceData &b) {
                  if (a.lastSeen != b.lastSeen) {
                    return a.lastSeen < b.lastSeen;
                  }
                  return a.rssi < b.rssi;
                });

      int devicesToRemove = MAX_DEVICES / 4;
      if (devicesToRemove > 0) {
        skimmerDevices.erase(skimmerDevices.begin(),
                            skimmerDevices.begin() + devicesToRemove);
      }

      currentIndex = listStartIndex = 0;
    }

    skimmerCallbackCount = 0;
    pBLEScan->start(SCAN_DURATION / 1000, false);
    isScanning = true;
    lastScanTime = now;
  }

  static bool showingRefresh = false;
  if (!isScanning && now - lastScanTime > SCAN_INTERVAL - 1000) {
    showingRefresh = true;
  } else if (isScanning && showingRefresh) {
    showingRefresh = false;
  }

  if (now - lastButtonPress > debounceTime) {
    if (!isDetailView && digitalRead(BTN_UP) == LOW && currentIndex > 0) {
      --currentIndex;
      if (currentIndex < listStartIndex)
        --listStartIndex;
      lastButtonPress = now;
    } else if (!isDetailView && digitalRead(BTN_DOWN) == LOW &&
               currentIndex < (int)skimmerDevices.size() - 1) {
      ++currentIndex;
      if (currentIndex >= listStartIndex + 5)
        ++listStartIndex;
      lastButtonPress = now;
    } else if (!isDetailView && digitalRead(BTN_RIGHT) == LOW &&
               !skimmerDevices.empty()) {
      isDetailView = true;
      lastButtonPress = now;
    } else if (digitalRead(BTN_BACK) == LOW) {
      isDetailView = false;
      lastButtonPress = now;
    } else if (digitalRead(BTN_CENTER) == LOW) {
      lastButtonPress = now;
      return;
    }
  }

  if (skimmerDevices.empty()) {
    currentIndex = listStartIndex = 0;
    isDetailView = false;
  } else {
    currentIndex = constrain(currentIndex, 0, (int)skimmerDevices.size() - 1);
    listStartIndex = constrain(listStartIndex, 0, max(0, (int)skimmerDevices.size() - 5));
  }

  u8g2.clearBuffer();
  if (showingRefresh) {
    u8g2.drawStr(0, 10, "Refreshing");
    u8g2.drawStr(0, 20, "Skimmers...");
    u8g2.sendBuffer();
  } else if (skimmerDevices.empty()) {
    u8g2.drawStr(0, 10, "Scanning for");
    u8g2.drawStr(0, 20, "Card Skimmers...");
    u8g2.drawStr(0, 35, "Nearby: n/a");
    u8g2.drawStr(0, 50, "Press CENTER to exit");
  } else if (isDetailView) {
    auto &dev = skimmerDevices[currentIndex];
    u8g2.setFont(u8g2_font_5x8_tr);
    char buf[32];
    snprintf(buf, sizeof(buf), "Name: %s", dev.name);
    u8g2.drawStr(0, 10, buf);
    snprintf(buf, sizeof(buf), "MAC: %s", dev.address);
    u8g2.drawStr(0, 20, buf);
    snprintf(buf, sizeof(buf), "RSSI: %d dBm", dev.rssi);
    u8g2.drawStr(0, 30, buf);
    snprintf(buf, sizeof(buf), "Age: %lus", (millis() - dev.lastSeen) / 1000);
    u8g2.drawStr(0, 40, buf);
    u8g2.drawStr(0, 60, "Press LEFT to go back");
  } else {
    u8g2.setFont(u8g2_font_6x10_tr);
    char header[32];
    snprintf(header, sizeof(header), "Skimmers: %d/%d",
             (int)skimmerDevices.size(), MAX_DEVICES);
    u8g2.drawStr(0, 10, header);

    for (int i = 0; i < 5; ++i) {
      int idx = listStartIndex + i;
      if (idx >= (int)skimmerDevices.size())
        break;
      auto &d = skimmerDevices[idx];
      if (idx == currentIndex)
        u8g2.drawStr(0, 20 + i * 10, ">");
      char line[32];
      snprintf(line, sizeof(line), "%.8s | RSSI %d",
               d.name[0] ? d.name : "Skimmer", d.rssi);
      u8g2.drawStr(10, 20 + i * 10, line);
    }
  }
  u8g2.sendBuffer();
}