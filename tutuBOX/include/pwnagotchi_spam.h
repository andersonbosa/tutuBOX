
#ifndef PWNAGOTCHI_SPAM_H
#define PWNAGOTCHI_SPAM_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include "pindefs.h"
#include "neopixel.h"

void pwnagotchiSpamSetup();
void pwnagotchiSpamLoop();

#endif