/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanBOX
   ________________________________________ */

#ifndef LEVEL_SYSTEM_H
#define LEVEL_SYSTEM_H

#include <Arduino.h>

void levelSystemSetup();
void levelSystemLoop();
void addXP(int amount);
int getCurrentLevel();
int getCurrentXP();
int getXPForNextLevel();
void displayLevelScreen();
void resetXPData();

#endif