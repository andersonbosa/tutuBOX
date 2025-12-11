/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#ifndef blescan_H
#define blescan_H

#include <vector>
#include <U8g2lib.h>
#include "neopixel.h"
#include "pindefs.h"

struct BLEDeviceData {
  char name[32];
  char address[18];
  uint8_t bdAddr[6];
  int8_t rssi;
  bool hasName;
  unsigned long lastSeen;
  uint8_t payload[64];
  size_t payloadLength;
  uint8_t scanResponse[64];
  size_t scanResponseLength;
  uint8_t advType;
  uint8_t addrType;
};

extern std::vector<BLEDeviceData> bleDevices;

void blescanSetup();
void blescanLoop();

#endif