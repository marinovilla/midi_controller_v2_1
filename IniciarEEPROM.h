#ifndef INICIAR_EEPROM_H
#define INICIAR_EEPROM_H

#include <EEPROM.h>
#include "config.h"

void ini_EEPROM() {
    EEPROM.begin(1024);
    for (int p = 0; p < NUM_PERFILES; p++) {
        for (int i = 0; i < NUM_PULSADORES; i++) {
            uint8_t valPuls = PERFIL_DEFECTO_PULSADORES[p][i];
            uint8_t valCC = PERFIL_DEFECTO_CC[p][i];
            uint8_t valModo = PERFIL_DEFECTO_MODO[p][i];
            EEPROM.write(ADDR_PULSADORES(p) + i, valPuls);
            EEPROM.write(ADDR_CC_PULS(p) + i, valCC);
            EEPROM.write(ADDR_MODO_PULS(p) + i, valModo);
            for (int r = 0; r < NUM_RELES; r++) {
                bool estado = PERFIL_DEFECTO_RELES[p][i][r];
                EEPROM.write(ADDR_RELES(p) + (i * NUM_RELES) + r, estado ? 1 : 0);
            }
        }
        for (int e = 0; e < NUM_EXPRESION; e++) {
            EEPROM.write(ADDR_CC_EXPRESION(p) + e, PERFIL_DEFECTO_CC_EXP[p][e]);
        }
        EEPROM.write(ADDR_PC_CERO(p), PERFIL_DEFECTO_PC_CERO[p]);
    }
    EEPROM.commit();
    EEPROM.end();
}

#endif
