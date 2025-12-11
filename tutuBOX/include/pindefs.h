// pindefs.h
#ifndef PINDEFS_H
#define PINDEFS_H

// Button Pin Definitions
#define BUTTON_PIN_UP      38
#define BUTTON_PIN_DOWN    36
#define BUTTON_PIN_CENTER  37  // Exit
#define BUTTON_PIN_LEFT    39  // Back
#define BUTTON_PIN_RIGHT   35  // Select

// Radio Pins
#define RADIO_CE_PIN_1      5 // marrom=branco
#define RADIO_CSN_PIN_1     6 // laranja=cinza
#define RADIO_CE_PIN_2      7
#define RADIO_CSN_PIN_2     15
#define RADIO_CE_PIN_3      16
#define RADIO_CSN_PIN_3     17

#define RADIO_SCK_PIN       18
#define RADIO_MISO_PIN      19
#define RADIO_MOSI_PIN      20 // esp32-s3 nao tem 23, troquei para 20
#define RADIO_SS_PIN        17

// NeoPixel
#define NEOPIXEL_PIN        14

// Display
#define DISPLAY_PIN_SCL 8
#define DISPLAY_PIN_SDA 9

#endif // PINDEFS_H
