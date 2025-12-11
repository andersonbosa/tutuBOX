/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________
*/

#ifndef BLE_SPAM_H
#define BLE_SPAM_H

#include <esp_bt.h>

extern bool isBleSpamming;

void bleSpamSetup();
void bleSpamLoop();

#endif
