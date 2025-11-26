#ifndef CONFIG_H
#define CONFIG_H

#define NUM_PULSADORES 6
#define NUM_RELES 2
#define NUM_EXPRESION 2

const int PINES_PULSADORES[NUM_PULSADORES] = {5, 6, 7, 15, 16, 3};
const int PINES_LEDS[NUM_PULSADORES] = {9, 10, 11, 12, 13, 46};
const int PINES_RELES[NUM_RELES] = {47, 48};
const int PINES_LEDS_RELES[NUM_RELES] = {20, 21};
const int PINES_EXPRESION[NUM_EXPRESION] = {1, 2};
const int PIN_MODO = 19;

bool ini_PC_zero = false;

#define ADDR_PULSADORES 0
#define ADDR_RELES 100
#define ADDR_CC_PULS 200
#define ADDR_MODO_PULS 300
#define ADDR_EXPRESION 400
#define ADDR_CC_EXPRESION 500
#define ADDR_MODO_EXPRESION 600
#define ADDR_ZERO 700

HardwareSerial Serial_MIDI(1);
#define TXD_PIN 17
#define RXD_PIN 18

#define AP_SSID "Mapo MIDI Control"
#define AP_PASSWORD "12345678"
#define HOST_NAME "mapomidi"
#define mdns "mapomidi"

#endif
