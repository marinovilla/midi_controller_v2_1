#ifndef MODO_FUNCIONAMIENTO_H
#define MODO_FUNCIONAMIENTO_H

#include <EEPROM.h>
#include "config.h"
#include "midi.h"
#include "ModoPerfiles.h"

int estado_pulsador[NUM_PULSADORES] = {HIGH};
int programas[NUM_PULSADORES] = {-1};
bool config_rele[NUM_PULSADORES][NUM_RELES] = {{false}};
int cc_pulsador[NUM_PULSADORES] = {-1};
int modo_pulsador[NUM_PULSADORES] = {0};
bool estado_cc[NUM_PULSADORES] = {false};
int cc_expresion[NUM_EXPRESION] = {0};
int exp_anterior[NUM_EXPRESION] = {-1};
bool estado_rele[NUM_RELES] = {false};
int ultimo_pulsador = -1;
int programa = -1;
int programa_anterior = -1;
bool zero_iniciar = true;

void ledsModoFuncionamiento() {
    for (int i = 0; i < NUM_PULSADORES; i++) {
        pinMode(PINES_LEDS[i], OUTPUT);
        digitalWrite(PINES_LEDS[i], HIGH);
        delay(200);
        digitalWrite(PINES_LEDS[i], LOW);
        delay(200);
    }
}

void aplicarReles(bool nuevos_estados[]) {
    for (int r = 0; r < NUM_RELES; r++) {
        if (estado_rele[r] != nuevos_estados[r]) {
            Serial.print("Rele");
            Serial.print(r + 1);
            Serial.println(nuevos_estados[r] ? " on" : " off");
        }
        estado_rele[r] = nuevos_estados[r];
        digitalWrite(PINES_RELES[r], nuevos_estados[r] ? HIGH : LOW);
        digitalWrite(PINES_LEDS_RELES[r], nuevos_estados[r] ? HIGH : LOW);
    }
}

void cargarConfiguracion(int perfil) {
    EEPROM.begin(EEPROM_TOTAL + 16);
    for (int i = 0; i < NUM_PULSADORES; i++) {
        programas[i] = EEPROM.read(ADDR_PULSADORES(perfil) + i);
        cc_pulsador[i] = EEPROM.read(ADDR_CC_PULS(perfil) + i);
        modo_pulsador[i] = EEPROM.read(ADDR_MODO_PULS(perfil) + i);
        for (int r = 0; r < NUM_RELES; r++)
            config_rele[i][r] = EEPROM.read(ADDR_RELES(perfil) + (i * NUM_RELES) + r) == 1;
    }
    for (int i = 0; i < NUM_EXPRESION; i++)
        cc_expresion[i] = EEPROM.read(ADDR_CC_EXP(perfil) + i);

    zero_iniciar = EEPROM.read(ADDR_PC_CERO(perfil)) == 1;
    EEPROM.end();
}

void setupModoFuncionamiento() {
    for (int i = 0; i < NUM_PULSADORES; i++) {
        pinMode(PINES_PULSADORES[i], INPUT_PULLUP);
        pinMode(PINES_LEDS[i], OUTPUT);
        digitalWrite(PINES_LEDS[i], LOW);
    }
    for (int r = 0; r < NUM_RELES; r++) {
        pinMode(PINES_RELES[r], OUTPUT);
        pinMode(PINES_LEDS_RELES[r], OUTPUT);
        digitalWrite(PINES_RELES[r], LOW);
        digitalWrite(PINES_LEDS_RELES[r], LOW);
    }
    for (int i = 0; i < NUM_EXPRESION; i++)
        pinMode(PINES_EXPRESION[i], INPUT);

    pinMode(PIN_PERFIL, INPUT_PULLUP);
    ledsModoFuncionamiento();
}

void Pulsadores() {
    for (int i = 0; i < NUM_PULSADORES; i++) {
        int estado_actual = digitalRead(PINES_PULSADORES[i]);
        if (estado_actual == LOW && estado_pulsador[i] == HIGH) {
            delay(30);
            if (digitalRead(PINES_PULSADORES[i]) == LOW) {
                if (modo_pulsador[i] == 0) {
                    programa = programas[i];
                    int pc_cmd = programa;
                    if (!zero_iniciar && pc_cmd > 0) pc_cmd += 1; // respeta la opción "PC en 0"
                    else if (zero_iniciar && pc_cmd > 0) pc_cmd -= 1;
                    programa_midi(pc_cmd);
                    programa_anterior = pc_cmd;

                    for (int j = 0; j < NUM_PULSADORES; j++)
                        digitalWrite(PINES_LEDS[j], (j == i) ? HIGH : LOW);

                    bool nuevos_estados[NUM_RELES];
                    for (int r = 0; r < NUM_RELES; r++)
                        nuevos_estados[r] = config_rele[i][r];
                    aplicarReles(nuevos_estados);

                    ultimo_pulsador = i;
                    for (int k = 0; k < NUM_PULSADORES; k++) {
                        estado_cc[k] = false;
                        if (modo_pulsador[k] == 1) digitalWrite(PINES_LEDS[k], LOW);
                    }
                } else if (modo_pulsador[i] == 1) {
                    estado_cc[i] = !estado_cc[i];
                    int valor = estado_cc[i] ? 127 : 0;
                    enviarExpresionMidi(valor, cc_pulsador[i]);

                    digitalWrite(PINES_LEDS[i], estado_cc[i] ? HIGH : LOW);
                    bool nuevos_estados[NUM_RELES];
                    for (int r = 0; r < NUM_RELES; r++) {
                        nuevos_estados[r] = estado_rele[r];
                        if (config_rele[i][r]) nuevos_estados[r] = !estado_rele[r];
                    }
                    aplicarReles(nuevos_estados);
                } else if (modo_pulsador[i] == 2) {
                    bool nuevos_estados[NUM_RELES];
                    for (int r = 0; r < NUM_RELES; r++) {
                        nuevos_estados[r] = estado_rele[r];
                        if (config_rele[i][r]) nuevos_estados[r] = !estado_rele[r];
                    }
                    aplicarReles(nuevos_estados);
                    digitalWrite(PINES_LEDS[i], HIGH);
                    delay(100);
                    digitalWrite(PINES_LEDS[i], LOW);
                }
            }
        }
        estado_pulsador[i] = estado_actual;
    }
}

void Expresion() {
    for (int i = 0; i < NUM_EXPRESION; i++) {
        int valor_pedal = map(analogRead(PINES_EXPRESION[i]), 0, 4095, 0, 127);
        if (abs(valor_pedal - exp_anterior[i]) > 3) {
            enviarExpresionMidi(valor_pedal, cc_expresion[i]);
            exp_anterior[i] = valor_pedal;
        }
    }
}

void verificarEntradaModoPerfil() {
    if (!estaModoPerfilActivo() && digitalRead(PIN_PERFIL) == LOW) {
        delay(50);
        if (digitalRead(PIN_PERFIL) == LOW) iniciarModoPerfil();
    }
}

void Modo_Funcionamiento() {
    setupModoFuncionamiento();

    EEPROM.begin(EEPROM_TOTAL + 16);
    int perfilDef = EEPROM.read(ADDR_PERFIL_DEFECTO);
    EEPROM.end();
    if (perfilDef < 0 || perfilDef >= NUM_PERFILES) perfilDef = 0;

    cargarConfiguracion(perfilDef);

    while (true) {
        verificarEntradaModoPerfil();
        if (estaModoPerfilActivo()) manejarModoPerfil();
        else {
            Pulsadores();
            Expresion();
        }
        delay(10);
    }
}

#endif
