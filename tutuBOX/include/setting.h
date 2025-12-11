/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/andersonbosa/tutuBOX/tutuBOX
   ________________________________________ */
   
#ifndef setting_H
#define setting_H

#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

extern bool neoPixelActive;
extern bool dangerousActionsEnabled;
extern bool continuousScanEnabled;

void settingSetup();
void settingLoop();
bool isDangerousActionsEnabled();
bool isContinuousScanEnabled();

#endif
