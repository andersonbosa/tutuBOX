/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________
*/

#ifndef DISPLAY_MIRROR_H
#define DISPLAY_MIRROR_H

#include <U8g2lib.h>

void displayMirrorSetup();

void displayMirrorSend(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display);

void displayMirrorEnable(bool enable);

bool displayMirrorEnabled();

#endif
