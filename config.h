#ifndef CONFIG_H
#define CONFIG_H

#define NUM_PULSADORES 6
#define NUM_RELES 2
#define NUM_EXPRESION 2
#define NUM_PERFILES 6
#define BLOQUE_PERFIL 50

const int PINES_PULSADORES[NUM_PULSADORES] = {5, 6, 7, 15, 16, 3};
const int PINES_LEDS[NUM_PULSADORES] = {9, 10, 11, 12, 13, 46};
const int PINES_RELES[NUM_RELES] = {47, 48};
const int PINES_LEDS_RELES[NUM_RELES] = {20, 21};
const int PINES_EXPRESION[NUM_EXPRESION] = {1, 2};
const int PIN_MODO = 19;
const int PIN_PERFIL = 4;

#define ADDR_PULSADORES_BASE 0
#define ADDR_RELES_BASE 12
#define ADDR_CC_PULS_BASE 16
#define ADDR_MODO_PULS_BASE 28
#define ADDR_EXPRESION_BASE 34
#define ADDR_CC_EXPRESION_BASE 38
#define ADDR_MODO_EXPRESION_BASE 42
#define ADDR_PC_CERO_BASE 46
#define ADDR_PERFIL_DEFECTO 300

#define ADDR_PULSADORES(p) (ADDR_PULSADORES_BASE + BLOQUE_PERFIL*(p))
#define ADDR_RELES(p) (ADDR_RELES_BASE + BLOQUE_PERFIL*(p))
#define ADDR_CC_PULS(p) (ADDR_CC_PULS_BASE + BLOQUE_PERFIL*(p))
#define ADDR_MODO_PULS(p) (ADDR_MODO_PULS_BASE + BLOQUE_PERFIL*(p))
#define ADDR_EXPRESION(p) (ADDR_EXPRESION_BASE + BLOQUE_PERFIL*(p))
#define ADDR_CC_EXPRESION(p) (ADDR_CC_EXPRESION_BASE + BLOQUE_PERFIL*(p))
#define ADDR_MODO_EXPRESION(p) (ADDR_MODO_EXPRESION_BASE + BLOQUE_PERFIL*(p))
#define ADDR_PC_CERO(p) (ADDR_PC_CERO_BASE + BLOQUE_PERFIL*(p))

HardwareSerial Serial_MIDI(1);
#define TXD_PIN 17
#define RXD_PIN 18

#define AP_SSID "Mapo MIDI Control"
#define AP_PASSWORD "12345678"
#define HOST_NAME "mapomidi"
#define mdns "mapomidi"


const uint8_t PERFIL_DEFECTO_PULSADORES[NUM_PERFILES][NUM_PULSADORES] = {
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 4, 5, 6}
};

const uint8_t PERFIL_DEFECTO_CC[NUM_PERFILES][NUM_PULSADORES] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

const uint8_t PERFIL_DEFECTO_MODO[NUM_PERFILES][NUM_PULSADORES] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

const bool PERFIL_DEFECTO_RELES[NUM_PERFILES][NUM_PULSADORES][NUM_RELES] = {{{false}}};

const uint8_t PERFIL_DEFECTO_CC_EXP[NUM_PERFILES][NUM_EXPRESION] = {
    {11, 4},
    {11, 4},
    {11, 4},
    {11, 4},
    {11, 4},
    {11, 4}
};

const uint8_t PERFIL_DEFECTO_PC_CERO[NUM_PERFILES] = {1, 1, 1, 1, 1, 1};

#endif
