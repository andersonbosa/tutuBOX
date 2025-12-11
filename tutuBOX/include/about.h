#ifndef ABOUT_H
#define ABOUT_H

#include <U8g2lib.h>
#include "pindefs.h"

#define TUTUBOX_VERSION "v1.0.0"
extern const char* tutuboxVersion;

void aboutSetup();
void aboutLoop();
void aboutCleanup();

#endif