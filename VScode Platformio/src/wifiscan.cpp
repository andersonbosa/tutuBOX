/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#include "../include/wifiscan.h"
#include "../include/sleep_manager.h"
#include "../include/display_mirror.h"
#include "../include/setting.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <vector>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

namespace {

#define BTN_UP BUTTON_PIN_UP
#define BTN_DOWN BUTTON_PIN_DOWN
#define BTN_RIGHT BUTTON_PIN_RIGHT
#define BTN_BACK BUTTON_PIN_LEFT

struct WiFiNetworkData {
  char ssid[33];
  char bssid[18];
  int8_t rssi;
  uint8_t channel;
  uint8_t encryption;
  unsigned long lastSeen;
  char authMode[20];
};
std::vector<WiFiNetworkData> wifiNetworks;

const int MAX_NETWORKS = 100;

int currentIndex = 0;
int listStartIndex = 0;
bool isDetailView = false;
bool isLocateMode = false;
char locateTargetBSSID[18] = {0};
uint8_t locateTargetChannel = 0;
unsigned long lastButtonPress = 0;
const unsigned long debounceTime = 200;

bool wifiscan_isScanning = false;
unsigned long wifiscan_lastScanTime = 0;
unsigned long wifiscan_scanStartTime = 0;
unsigned long wifiscan_lastDisplayUpdate = 0;
const unsigned long scanInterval = 180000;
const unsigned long scanDuration = 8000;
const unsigned long displayUpdateInterval = 100;

bool wifiscan_scanCompleted = false;
uint16_t wifiscan_lastApCount = 0;

static bool needsRedraw = true;
static int lastNetworkCount = 0;
static unsigned long lastLocateUpdate = 0;
const unsigned long locateUpdateInterval = 1000;
static unsigned long lastCountdownUpdate = 0;
const unsigned long countdownUpdateInterval = 1000;
static bool wasScanning = false;

const char* getAuthModeString(wifi_auth_mode_t authMode) {
    switch (authMode) {
        case WIFI_AUTH_OPEN: return "Open";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA-PSK";
        case WIFI_AUTH_WPA2_PSK: return "WPA2-PSK";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Ent";
        case WIFI_AUTH_WPA3_PSK: return "WPA3-PSK";
        case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3";
        case WIFI_AUTH_WAPI_PSK: return "WAPI-PSK";
        default: return "Unknown";
    }
}

void bssid_to_string(uint8_t *bssid, char *str, size_t size) {
    if (bssid == NULL || str == NULL || size < 18) {
        return;
    }
    snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
}

void processScanResults(unsigned long now) {
    uint16_t number = 0;
    esp_wifi_scan_get_ap_num(&number);
    
    if (number == 0) return;
    
    wifi_ap_record_t *ap_info = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * number);
    
    if (ap_info == NULL) return;
    
    memset(ap_info, 0, sizeof(wifi_ap_record_t) * number);
    
    uint16_t actual_number = number;
    esp_err_t err = esp_wifi_scan_get_ap_records(&actual_number, ap_info);
    
    if (err == ESP_OK) {
        for (int i = 0; i < actual_number; i++) {
            if (ap_info[i].ssid[0] == '\0') {
                continue;
            }

            char bssidStr[18];
            bssid_to_string(ap_info[i].bssid, bssidStr, sizeof(bssidStr));

            if (isLocateMode && strlen(locateTargetBSSID) > 0) {
                if (strcmp(bssidStr, locateTargetBSSID) != 0) {
                    continue;
                }
            } else if (wifiNetworks.size() >= MAX_NETWORKS) {
                continue;
            }

            bool found = false;
            for (auto &net : wifiNetworks) {
                if (strcmp(net.bssid, bssidStr) == 0) {
                    net.rssi = ap_info[i].rssi;
                    net.lastSeen = now;
                    strncpy(net.ssid, (char*)ap_info[i].ssid, sizeof(net.ssid) - 1);
                    net.ssid[sizeof(net.ssid) - 1] = '\0';
                    found = true;
                    break;
                }
            }

            if (!found) {
                WiFiNetworkData newNetwork;
                memset(&newNetwork, 0, sizeof(newNetwork));
                strncpy(newNetwork.bssid, bssidStr, sizeof(newNetwork.bssid) - 1);
                newNetwork.bssid[sizeof(newNetwork.bssid) - 1] = '\0';
                newNetwork.rssi = ap_info[i].rssi;
                newNetwork.channel = ap_info[i].primary;
                newNetwork.encryption = ap_info[i].authmode;
                newNetwork.lastSeen = now;
                
                strncpy(newNetwork.authMode, getAuthModeString(ap_info[i].authmode), sizeof(newNetwork.authMode) - 1);
                newNetwork.authMode[sizeof(newNetwork.authMode) - 1] = '\0';
                
                strncpy(newNetwork.ssid, (char*)ap_info[i].ssid, sizeof(newNetwork.ssid) - 1);
                newNetwork.ssid[sizeof(newNetwork.ssid) - 1] = '\0';
                
                wifiNetworks.push_back(newNetwork);
            }
        }

        if (!isLocateMode) {
            std::sort(wifiNetworks.begin(), wifiNetworks.end(),
                    [](const WiFiNetworkData &a, const WiFiNetworkData &b) {
                        return a.rssi > b.rssi;
                    });
        }
    }
    
    free(ap_info);
}

}

void wifiscanSetup() {
  wifiNetworks.clear();
  wifiNetworks.reserve(MAX_NETWORKS);
  currentIndex = listStartIndex = 0;
  isDetailView = false;
  isLocateMode = false;
  memset(locateTargetBSSID, 0, sizeof(locateTargetBSSID));
  locateTargetChannel = 0;
  lastButtonPress = 0;
  wifiscan_isScanning = false;
  wifiscan_scanCompleted = false;
  wifiscan_lastApCount = 0;
  wifiscan_lastDisplayUpdate = 0;
  needsRedraw = true;
  lastNetworkCount = 0;
  lastLocateUpdate = 0;
  lastCountdownUpdate = 0;
  wasScanning = false;

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Scanning for");
  u8g2.drawStr(0, 20, "WiFi networks...");
  char countStr[32];
  snprintf(countStr, sizeof(countStr), "%d/%d networks", 0, MAX_NETWORKS);
  u8g2.drawStr(0, 35, countStr);
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(0, 60, "Press SEL to exit");
  u8g2.sendBuffer();
  displayMirrorSend(u8g2);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();
  
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  
  // Start scan
  wifi_scan_config_t scan_config = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = false,
    .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    .scan_time = {
      .active = {
        .min = 120,
        .max = 200
      }
    }
  };
  
  esp_wifi_scan_start(&scan_config, false);
  wifiscan_isScanning = true;
  wifiscan_scanStartTime = millis();
  wifiscan_lastScanTime = millis();
}

void wifiscanLoop() {
  unsigned long now = millis();

  unsigned long effectiveScanInterval = scanInterval;
  unsigned long effectiveScanDuration = scanDuration;

  if (wifiNetworks.empty() && isContinuousScanEnabled() && wifiscan_scanCompleted) {
    effectiveScanInterval = 500;
    effectiveScanDuration = 3000;
  }

  bool shouldShowScanningScreen = wifiscan_isScanning || (wifiNetworks.empty() && isContinuousScanEnabled());

  if (shouldShowScanningScreen) {
    uint16_t currentApCount = 0;
    esp_wifi_scan_get_ap_num(&currentApCount);

    if (currentApCount > wifiscan_lastApCount) {
      processScanResults(now);
      wifiscan_lastApCount = currentApCount;
    }

    if (isLocateMode && now - wifiscan_lastDisplayUpdate > displayUpdateInterval) {
      processScanResults(now);
      wifiscan_lastDisplayUpdate = now;
    }

    if (wifiscan_isScanning && now - wifiscan_scanStartTime > effectiveScanDuration) {
      processScanResults(now);

      wifiscan_lastScanTime = now;
      wifiscan_lastApCount = 0;
      esp_wifi_scan_stop();

      if (isLocateMode) {
        wifi_scan_config_t scan_config = {
          .ssid = NULL,
          .bssid = NULL,
          .channel = locateTargetChannel,
          .show_hidden = false,
          .scan_type = WIFI_SCAN_TYPE_ACTIVE,
          .scan_time = {
            .active = {
              .min = 120,
              .max = 200
            }
          }
        };
        esp_wifi_scan_start(&scan_config, false);
        wifiscan_isScanning = true;
        wifiscan_scanStartTime = now;
      } else {
        wifiscan_isScanning = false;
        wifiscan_scanCompleted = true;
      }
    }

    if (!isLocateMode) {
      if (lastNetworkCount != (int)wifiNetworks.size() || wasScanning != wifiscan_isScanning) {
        lastNetworkCount = (int)wifiNetworks.size();
        wasScanning = wifiscan_isScanning;

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.drawStr(0, 10, "Scanning for");
        u8g2.drawStr(0, 20, "WiFi networks...");

        char countStr[32];
        snprintf(countStr, sizeof(countStr), "%d/%d networks", (int)wifiNetworks.size(), MAX_NETWORKS);
        u8g2.drawStr(0, 35, countStr);

        int barWidth = 120;
        int barHeight = 10;
        int barX = (128 - barWidth) / 2;
        int barY = 42;

        u8g2.drawFrame(barX, barY, barWidth, barHeight);

        int fillWidth = (wifiNetworks.size() * (barWidth - 4)) / MAX_NETWORKS;
        if (fillWidth > 0) {
          u8g2.drawBox(barX + 2, barY + 2, fillWidth, barHeight - 4);
        }

        u8g2.setFont(u8g2_font_5x8_tr);
        u8g2.drawStr(0, 62, "Press SEL to exit");
        u8g2.sendBuffer();
        displayMirrorSend(u8g2);
      }
      return;
    }
  }

  if (wasScanning != wifiscan_isScanning) {
    wasScanning = wifiscan_isScanning;
    needsRedraw = true;
  }

  if (!wifiscan_isScanning && wifiscan_scanCompleted && now - wifiscan_lastScanTime > effectiveScanInterval &&
      !isDetailView && !isLocateMode) {
    if (wifiNetworks.size() >= MAX_NETWORKS) {
      std::sort(wifiNetworks.begin(), wifiNetworks.end(),
              [](const WiFiNetworkData &a, const WiFiNetworkData &b) {
                  if (a.lastSeen != b.lastSeen) {
                      return a.lastSeen < b.lastSeen;
                  }
                  return a.rssi < b.rssi;
              });
      
      int networksToRemove = MAX_NETWORKS / 4;
      if (networksToRemove > 0) {
          wifiNetworks.erase(wifiNetworks.begin(), 
                            wifiNetworks.begin() + networksToRemove);
      }
      
      currentIndex = listStartIndex = 0;
    }
    
    wifi_scan_config_t scan_config = {
      .ssid = NULL,
      .bssid = NULL,
      .channel = 0,
      .show_hidden = false,
      .scan_type = WIFI_SCAN_TYPE_ACTIVE,
      .scan_time = {
        .active = {
          .min = 120,
          .max = 200
        }
      }
    };
    
    wifiscan_scanCompleted = false;
    esp_wifi_scan_start(&scan_config, false);
    wifiscan_isScanning = true;
    wifiscan_scanStartTime = now;
    wifiscan_lastScanTime = now;
    wifiscan_lastApCount = 0;
    return;
  }

  if (wifiscan_scanCompleted && now - lastButtonPress > debounceTime) {
    if (!isDetailView && !isLocateMode && digitalRead(BTN_UP) == LOW && currentIndex > 0) {
      --currentIndex;
      if (currentIndex < listStartIndex)
        --listStartIndex;
      lastButtonPress = now;
      needsRedraw = true;
    } else if (!isDetailView && !isLocateMode && digitalRead(BTN_DOWN) == LOW &&
               currentIndex < (int)wifiNetworks.size() - 1) {
      ++currentIndex;
      if (currentIndex >= listStartIndex + 5)
        ++listStartIndex;
      lastButtonPress = now;
      needsRedraw = true;
    } else if (!isDetailView && !isLocateMode && digitalRead(BTN_RIGHT) == LOW &&
               !wifiNetworks.empty()) {
      isDetailView = true;
      lastButtonPress = now;
      needsRedraw = true;
    } else if (isDetailView && !isLocateMode && digitalRead(BTN_RIGHT) == LOW &&
               !wifiNetworks.empty()) {
      isLocateMode = true;
      strncpy(locateTargetBSSID, wifiNetworks[currentIndex].bssid, sizeof(locateTargetBSSID) - 1);
      locateTargetBSSID[sizeof(locateTargetBSSID) - 1] = '\0';
      locateTargetChannel = wifiNetworks[currentIndex].channel;
      if (!wifiscan_isScanning) {
        wifi_scan_config_t scan_config = {
          .ssid = NULL,
          .bssid = NULL,
          .channel = locateTargetChannel,
          .show_hidden = false,
          .scan_type = WIFI_SCAN_TYPE_ACTIVE,
          .scan_time = {
            .active = {
              .min = 120,
              .max = 200
            }
          }
        };
        esp_wifi_scan_start(&scan_config, false);
        wifiscan_isScanning = true;
        wifiscan_scanStartTime = now;
      }
      lastButtonPress = now;
      lastLocateUpdate = now;
      needsRedraw = true;
    } else if (isLocateMode && digitalRead(BTN_BACK) == LOW) {
      isLocateMode = false;
      memset(locateTargetBSSID, 0, sizeof(locateTargetBSSID));
      locateTargetChannel = 0;
      if (wifiscan_isScanning) {
        esp_wifi_scan_stop();
        wifiscan_isScanning = false;
      }
      lastButtonPress = now;
      needsRedraw = true;
    } else if (isDetailView && !isLocateMode && digitalRead(BTN_BACK) == LOW) {
      isDetailView = false;
      lastButtonPress = now;
      needsRedraw = true;
    }
  }

  if (wifiNetworks.empty()) {
    if (currentIndex != 0 || isDetailView || isLocateMode) {
      needsRedraw = true;
    }
    currentIndex = listStartIndex = 0;
    isDetailView = false;
    isLocateMode = false;
    memset(locateTargetBSSID, 0, sizeof(locateTargetBSSID));
    locateTargetChannel = 0;
  } else {
    currentIndex = constrain(currentIndex, 0, (int)wifiNetworks.size() - 1);
    listStartIndex =
        constrain(listStartIndex, 0, max(0, (int)wifiNetworks.size() - 5));
  }

  if (isDetailView && now - lastLocateUpdate >= locateUpdateInterval) {
    lastLocateUpdate = now;
    needsRedraw = true;
  }

  if (isLocateMode && now - lastLocateUpdate >= locateUpdateInterval) {
    lastLocateUpdate = now;
    needsRedraw = true;
  }

  if (wifiNetworks.empty() && wifiscan_scanCompleted && !wifiscan_isScanning &&
      now - lastCountdownUpdate >= countdownUpdateInterval) {
    lastCountdownUpdate = now;
    needsRedraw = true;
  }

  if (!needsRedraw) {
    return;
  }

  needsRedraw = false;
  u8g2.clearBuffer();

  if (wifiNetworks.empty()) {
    if (isContinuousScanEnabled()) {
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawStr(0, 10, "Scanning for");
      u8g2.drawStr(0, 20, "WiFi networks...");

      char countStr[32];
      snprintf(countStr, sizeof(countStr), "%d/%d networks", 0, MAX_NETWORKS);
      u8g2.drawStr(0, 35, countStr);

      int barWidth = 120;
      int barHeight = 10;
      int barX = (128 - barWidth) / 2;
      int barY = 42;
      u8g2.drawFrame(barX, barY, barWidth, barHeight);

      u8g2.setFont(u8g2_font_5x8_tr);
      u8g2.drawStr(0, 62, "Press SEL to exit");
    } else {
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawStr(0, 10, "No networks found");
      u8g2.setFont(u8g2_font_5x8_tr);
      char timeStr[32];
      unsigned long timeLeft = (scanInterval - (now - wifiscan_lastScanTime)) / 1000;
      snprintf(timeStr, sizeof(timeStr), "Scanning in %lus", timeLeft);
      u8g2.drawStr(0, 30, timeStr);
      u8g2.drawStr(0, 45, "Press SEL to exit");
    }
  } else if (isLocateMode) {
    auto &net = wifiNetworks[currentIndex];
    u8g2.setFont(u8g2_font_5x8_tr);
    char buf[32];

    snprintf(buf, sizeof(buf), "%.13s Ch:%d", net.ssid, locateTargetChannel);
    u8g2.drawStr(0, 8, buf);

    snprintf(buf, sizeof(buf), "%s", net.bssid);
    u8g2.drawStr(0, 16, buf);

    u8g2.setFont(u8g2_font_7x13B_tr);
    snprintf(buf, sizeof(buf), "RSSI: %d dBm", net.rssi);
    u8g2.drawStr(0, 28, buf);

    u8g2.setFont(u8g2_font_5x8_tr);
    int rssiClamped = constrain(net.rssi, -100, -40);
    int signalLevel = map(rssiClamped, -100, -40, 0, 5);

    const char* quality;
    if (signalLevel >= 5) quality = "EXCELLENT";
    else if (signalLevel >= 4) quality = "VERY GOOD";
    else if (signalLevel >= 3) quality = "GOOD";
    else if (signalLevel >= 2) quality = "FAIR";
    else if (signalLevel >= 1) quality = "WEAK";
    else quality = "VERY WEAK";

    snprintf(buf, sizeof(buf), "Signal: %s", quality);
    u8g2.drawStr(0, 38, buf);

    int barWidth = 12;
    int barSpacing = 5;
    int totalWidth = (barWidth * 5) + (barSpacing * 4);
    int startX = (128 - totalWidth) / 2;
    int baseY = 54;

    for (int i = 0; i < 5; i++) {
      int barHeight = 8 + (i * 2);
      int x = startX + (i * (barWidth + barSpacing));
      int y = baseY - barHeight;

      if (i < signalLevel) {
        u8g2.drawBox(x, y, barWidth, barHeight);
      } else {
        u8g2.drawFrame(x, y, barWidth, barHeight);
      }
    }

    u8g2.drawStr(0, 62, "L=Back SEL=Exit");
  } else if (isDetailView) {
    auto &net = wifiNetworks[currentIndex];
    u8g2.setFont(u8g2_font_5x8_tr);
    char buf[40];

    snprintf(buf, sizeof(buf), "SSID: %s", net.ssid);
    u8g2.drawStr(0, 10, buf);

    snprintf(buf, sizeof(buf), "BSSID: %s", net.bssid);
    u8g2.drawStr(0, 20, buf);

    snprintf(buf, sizeof(buf), "RSSI: %d dBm", net.rssi);
    u8g2.drawStr(0, 30, buf);

    snprintf(buf, sizeof(buf), "Ch: %d  Auth: %s", net.channel, net.authMode);
    u8g2.drawStr(0, 40, buf);

    snprintf(buf, sizeof(buf), "Age: %lus", (now - net.lastSeen) / 1000);
    u8g2.drawStr(0, 50, buf);

    u8g2.drawStr(0, 62, "L=Back SEL=Exit R=Locate");
  } else {
    u8g2.setFont(u8g2_font_6x10_tr);
    char header[32];
    snprintf(header, sizeof(header), "WiFi: %d/%d", (int)wifiNetworks.size(), MAX_NETWORKS);
    u8g2.drawStr(0, 10, header);
    
    for (int i = 0; i < 5; ++i) {
      int idx = listStartIndex + i;
      if (idx >= (int)wifiNetworks.size())
        break;
      auto &n = wifiNetworks[idx];
      if (idx == currentIndex)
        u8g2.drawStr(0, 20 + i * 10, ">");
      char line[32];
      snprintf(line, sizeof(line), "%.8s | RSSI %d",
               n.ssid[0] ? n.ssid : "Unknown", n.rssi);
      u8g2.drawStr(10, 20 + i * 10, line);
    }
  }
  u8g2.sendBuffer();
  displayMirrorSend(u8g2);
}