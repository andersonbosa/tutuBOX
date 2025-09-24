/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________
*/

#ifndef airtag_H
#define airtag_H

#include <BLEDevice.h>
#include <U8g2lib.h>
#include "neopixel.h"
#include "pindefs.h"

void airtagDetectorSetup();
void airtagDetectorLoop();

#endif