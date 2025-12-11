/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#ifndef FLOCK_DETECTOR_H
#define FLOCK_DETECTOR_H

#include <U8g2lib.h>
#include "neopixel.h"
#include "pindefs.h"

void flockDetectorSetup();
void flockDetectorLoop();
void cleanupFlockDetector();

#endif
