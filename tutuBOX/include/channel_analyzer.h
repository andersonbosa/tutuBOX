/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/andersonbosa/tutuBOX/tutuBOX
   ________________________________________
*/

#ifndef CHANNEL_MONITOR_H
#define CHANNEL_MONITOR_H

#include <U8g2lib.h>
#include <Arduino.h>
#include "pindefs.h"

void channelAnalyzerSetup();
void channelAnalyzerLoop();

void drawNetworkCountView();
void drawSignalStrengthView();
const char* getSignalStrengthLabel(int rssi);

#endif