/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/andersonbosa/tutuBOX/tutuBOX
   ________________________________________
*/

#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

extern void updateLastActivity();
extern void checkIdle();
extern void wakeDisplay();
extern bool anyButtonPressed();
extern void updateSleepTimeout(unsigned long newTimeout);

#endif