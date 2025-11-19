/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#include <EEPROM.h>
#include <U8g2lib.h>
#include <Arduino.h>

#include "../include/setting.h"
#include "../include/sleep_manager.h"
#include "../include/display_mirror.h"
#include "../include/level_system.h"
#include "../include/legal_disclaimer.h"
#include "../include/pindefs.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

#define EEPROM_ADDRESS_NEOPIXEL 0
#define EEPROM_ADDRESS_BRIGHTNESS 1
#define EEPROM_ADDRESS_DANGEROUS_MODE 2
#define EEPROM_ADDRESS_SLEEP_TIMEOUT 3
#define EEPROM_ADDRESS_CONTINUOUS_SCAN 4

int currentSetting = 0;
int totalSettings = 6;
bool neoPixelActive = true;
uint8_t oledBrightness = 100;
extern bool dangerousActionsEnabled;
bool continuousScanEnabled = true;
bool showResetConfirm = false;
uint8_t sleepTimeoutIndex = 3;

static bool needsRedraw = true;
static int lastCurrentSetting = -1;
static bool lastNeoPixelActive = true;
static uint8_t lastOledBrightness = 100;
static bool lastDangerousActionsEnabled = false;
static bool lastContinuousScanEnabled = true;
static bool lastShowResetConfirm = false;
static uint8_t lastSleepTimeoutIndex = 3;

const unsigned long sleepTimeouts[] = {15, 30, 60, 120, 300, 900, 1800, 0};
const char* sleepTimeoutNames[] = {"15s", "30s", "1m", "2m", "5m", "15m", "30m", "Off"};
const int sleepTimeoutCount = 8;

extern unsigned long idleTimeout;
extern void updateSleepTimeout(unsigned long newTimeout);

void handleDangerousActions() {
  if (!dangerousActionsEnabled) {
    if (showLegalDisclaimer()) {
      dangerousActionsEnabled = true;
      EEPROM.write(EEPROM_ADDRESS_DANGEROUS_MODE, 1);
      EEPROM.commit();
    }
  } else {
    dangerousActionsEnabled = false;
    EEPROM.write(EEPROM_ADDRESS_DANGEROUS_MODE, 0);
    EEPROM.commit();
  }
}


void settingSetup() {
  uint8_t neoPixelValue = EEPROM.read(EEPROM_ADDRESS_NEOPIXEL);
  uint8_t brightnessValue = EEPROM.read(EEPROM_ADDRESS_BRIGHTNESS);
  uint8_t sleepTimeoutValue = EEPROM.read(EEPROM_ADDRESS_SLEEP_TIMEOUT);
  uint8_t continuousScanValue = EEPROM.read(EEPROM_ADDRESS_CONTINUOUS_SCAN);

  if (neoPixelValue == 0xFF) {
    neoPixelActive = true;
    EEPROM.write(EEPROM_ADDRESS_NEOPIXEL, 1);
    EEPROM.commit();
  } else {
    neoPixelActive = (neoPixelValue == 1);
  }

  if (brightnessValue > 255) {
    oledBrightness = 128;
  } else {
    oledBrightness = brightnessValue;
  }

  if (sleepTimeoutValue == 0xFF || sleepTimeoutValue >= sleepTimeoutCount) {
    sleepTimeoutIndex = 3;
    EEPROM.write(EEPROM_ADDRESS_SLEEP_TIMEOUT, sleepTimeoutIndex);
    EEPROM.commit();
  } else {
    sleepTimeoutIndex = sleepTimeoutValue;
  }

  if (continuousScanValue == 0xFF) {
    continuousScanEnabled = true;
    EEPROM.write(EEPROM_ADDRESS_CONTINUOUS_SCAN, 1);
    EEPROM.commit();
  } else {
    continuousScanEnabled = (continuousScanValue == 1);
  }

  u8g2.setContrast(oledBrightness);

  updateSleepTimeout(sleepTimeouts[sleepTimeoutIndex] * 1000);

  currentSetting = 0;
  showResetConfirm = false;

  needsRedraw = true;
  lastCurrentSetting = -1;
  lastNeoPixelActive = neoPixelActive;
  lastOledBrightness = oledBrightness;
  lastDangerousActionsEnabled = dangerousActionsEnabled;
  lastContinuousScanEnabled = continuousScanEnabled;
  lastShowResetConfirm = false;
  lastSleepTimeoutIndex = sleepTimeoutIndex;
}

void settingLoop() {
  static bool upPressed = false;
  static bool downPressed = false;
  static bool rightPressed = false;
  static bool leftPressed = false;
  static unsigned long lastUpPress = 0;
  static unsigned long lastDownPress = 0;
  static unsigned long lastRightPress = 0;
  static unsigned long lastLeftPress = 0;
  const unsigned long debounceDelay = 200;

  checkIdle();

  unsigned long now = millis();
  bool up = !digitalRead(BUTTON_PIN_UP);
  bool down = !digitalRead(BUTTON_PIN_DOWN);
  bool right = !digitalRead(BUTTON_PIN_RIGHT);
  bool left = !digitalRead(BUTTON_PIN_LEFT);

  if (up) {
    if (!upPressed && (now - lastUpPress > debounceDelay)) {
      upPressed = true;
      lastUpPress = now;
      if (!showResetConfirm) {
        currentSetting = (currentSetting - 1 + totalSettings) % totalSettings;
        needsRedraw = true;
      }
    }
  } else {
    upPressed = false;
  }

  if (down) {
    if (!downPressed && (now - lastDownPress > debounceDelay)) {
      downPressed = true;
      lastDownPress = now;
      if (!showResetConfirm) {
        currentSetting = (currentSetting + 1) % totalSettings;
        needsRedraw = true;
      }
    }
  } else {
    downPressed = false;
  }

  if (right) {
    if (!rightPressed && (now - lastRightPress > debounceDelay)) {
      rightPressed = true;
      lastRightPress = now;

      if (showResetConfirm) {
        resetXPData();
        showResetConfirm = false;
        needsRedraw = true;
      } else {
        switch (currentSetting) {
          case 0:
            neoPixelActive = !neoPixelActive;
            EEPROM.write(EEPROM_ADDRESS_NEOPIXEL, neoPixelActive ? 1 : 0);
            EEPROM.commit();
            needsRedraw = true;
            break;

          case 1:
            {
              uint8_t percent = map(oledBrightness, 0, 255, 0, 100);
              percent += 10;
              if (percent > 100) percent = 0;
              oledBrightness = map(percent, 0, 100, 0, 255);
              u8g2.setContrast(oledBrightness);
              EEPROM.write(EEPROM_ADDRESS_BRIGHTNESS, oledBrightness);
              EEPROM.commit();
              needsRedraw = true;
            }
            break;

          case 2:
            handleDangerousActions();
            needsRedraw = true;
            break;

          case 3:
            sleepTimeoutIndex = (sleepTimeoutIndex + 1) % sleepTimeoutCount;
            EEPROM.write(EEPROM_ADDRESS_SLEEP_TIMEOUT, sleepTimeoutIndex);
            EEPROM.commit();
            updateSleepTimeout(sleepTimeouts[sleepTimeoutIndex] * 1000);
            needsRedraw = true;
            break;

          case 4:
            continuousScanEnabled = !continuousScanEnabled;
            EEPROM.write(EEPROM_ADDRESS_CONTINUOUS_SCAN, continuousScanEnabled ? 1 : 0);
            EEPROM.commit();
            needsRedraw = true;
            break;

          case 5:
            showResetConfirm = true;
            needsRedraw = true;
            break;
        }
      }
    }
  } else {
    rightPressed = false;
  }

  if (left) {
    if (!leftPressed && (now - lastLeftPress > debounceDelay)) {
      leftPressed = true;
      lastLeftPress = now;
      if (showResetConfirm) {
        showResetConfirm = false;
        needsRedraw = true;
      }
    }
  } else {
    leftPressed = false;
  }

  if (lastCurrentSetting != currentSetting) {
    lastCurrentSetting = currentSetting;
    needsRedraw = true;
  }
  if (lastNeoPixelActive != neoPixelActive) {
    lastNeoPixelActive = neoPixelActive;
    needsRedraw = true;
  }
  if (lastOledBrightness != oledBrightness) {
    lastOledBrightness = oledBrightness;
    needsRedraw = true;
  }
  if (lastDangerousActionsEnabled != dangerousActionsEnabled) {
    lastDangerousActionsEnabled = dangerousActionsEnabled;
    needsRedraw = true;
  }
  if (lastShowResetConfirm != showResetConfirm) {
    lastShowResetConfirm = showResetConfirm;
    needsRedraw = true;
  }
  if (lastSleepTimeoutIndex != sleepTimeoutIndex) {
    lastSleepTimeoutIndex = sleepTimeoutIndex;
    needsRedraw = true;
  }
  if (lastContinuousScanEnabled != continuousScanEnabled) {
    lastContinuousScanEnabled = continuousScanEnabled;
    needsRedraw = true;
  }

  if (!needsRedraw) {
    return;
  }

  needsRedraw = false;
  u8g2.clearBuffer();

  if (showResetConfirm) {
    u8g2.setFont(u8g2_font_helvB08_tr);
    int titleWidth = u8g2.getUTF8Width("Reset XP Data?");
    u8g2.drawStr((128 - titleWidth) / 2, 20, "Reset XP Data?");

    u8g2.setFont(u8g2_font_6x10_tr);
    int messageWidth = u8g2.getUTF8Width("Reset to Level 1");
    u8g2.drawStr((128 - messageWidth) / 2, 35, "Reset to Level 1");

    u8g2.setFont(u8g2_font_4x6_tr);
    int buttonWidth = u8g2.getUTF8Width("LEFT=Cancel  RIGHT=Confirm");
    u8g2.drawStr((128 - buttonWidth) / 2, 55, "LEFT=Cancel  RIGHT=Confirm");
  } else {
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(45, 12, "Settings");

    u8g2.setFont(u8g2_font_6x10_tr);

    int startIndex = max(0, min(currentSetting - 1, totalSettings - 4));
    int displayIndex = 0;

    for (int i = startIndex; i < min(startIndex + 4, totalSettings); i++) {
      int yPos = 25 + displayIndex * 12;

      if (currentSetting == i) u8g2.drawStr(2, yPos, ">");

      switch (i) {
        case 0:
          u8g2.drawStr(10, yPos, "NeoPixel:");
          u8g2.drawStr(85, yPos, neoPixelActive ? "On" : "Off");
          break;
        case 1:
          u8g2.drawStr(10, yPos, "Brightness:");
          char brightStr[8];
          sprintf(brightStr, "%d%%", (int)map(oledBrightness, 0, 255, 0, 100));
          u8g2.drawStr(85, yPos, brightStr);
          break;
        case 2:
          u8g2.drawStr(10, yPos, "Dangerous:");
          u8g2.drawStr(85, yPos, dangerousActionsEnabled ? "On" : "Off");
          break;
        case 3:
          u8g2.drawStr(10, yPos, "Sleep:");
          u8g2.drawStr(85, yPos, sleepTimeoutNames[sleepTimeoutIndex]);
          break;
        case 4:
          u8g2.drawStr(10, yPos, "Fast Retry:");
          u8g2.drawStr(85, yPos, continuousScanEnabled ? "On" : "Off");
          break;
        case 5:
          u8g2.drawStr(10, yPos, "Reset XP:");
          char lvlStr[8];
          sprintf(lvlStr, "Lv%d", getCurrentLevel());
          u8g2.drawStr(85, yPos, lvlStr);
          break;
      }
      displayIndex++;
    }
  }

  u8g2.sendBuffer();
  displayMirrorSend(u8g2);
}

bool isDangerousActionsEnabled() {
  return dangerousActionsEnabled;
}

bool isContinuousScanEnabled() {
  return continuousScanEnabled;
}