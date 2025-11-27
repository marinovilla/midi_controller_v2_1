#ifndef INICIAR_EEPROM_H
#define INICIAR_EEPROM_H

#include <EEPROM.h>
#include "config.h"

void ini_EEPROM() {
    EEPROM.begin(EEPROM_TOTAL + 16);
    for(int p=0; p<NUM_PERFILES; p++){
        for(int i=0; i<NUM_PULSADORES; i++){
            EEPROM.write(ADDR_PULSADORES(p)+i, PERFIL_DEFECTO_PULSADORES[p][i]);
            EEPROM.write(ADDR_CC_PULS(p)+i, PERFIL_DEFECTO_CC[p][i]);
            EEPROM.write(ADDR_MODO_PULS(p)+i, PERFIL_DEFECTO_MODO[p][i]);
            for(int r=0; r<NUM_RELES; r++){
                EEPROM.write(ADDR_RELES(p)+(i*NUM_RELES)+r, PERFIL_DEFECTO_RELES[p][i][r]?1:0);
            }
        }
        for(int e=0; e<NUM_EXPRESION; e++){
            EEPROM.write(ADDR_CC_EXP(p)+e, PERFIL_DEFECTO_CC_EXP[p][e]);
        }
        EEPROM.write(ADDR_PC_CERO(p), PERFIL_DEFECTO_PC_CERO[p]);
    }
    EEPROM.write(ADDR_PERFIL_DEFECTO, 0);
    EEPROM.commit();
    EEPROM.end();
}

#endif
