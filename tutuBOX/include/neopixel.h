/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */
#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel pixels;

void neopixelSetup();
void neopixelLoop();

void blinkColor(uint8_t r, uint8_t g, uint8_t b);
void stopBlinking();

#endif // NEOPIXEL_H
