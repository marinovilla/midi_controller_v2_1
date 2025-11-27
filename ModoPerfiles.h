#ifndef MODO_PERFILES_H
#define MODO_PERFILES_H

#include <EEPROM.h>
#include "config.h"

extern void cargarConfiguracion(int perfil);

static bool _modoPerfilActivo = false;
static int _perfilSeleccionado = 0;
static unsigned long _lastBlink = 0;
static bool _blinkState = false;

void iniciarModoPerfil()
{
    EEPROM.begin(EEPROM_TOTAL + 16);
    int pdef = EEPROM.read(ADDR_PERFIL_DEFECTO);
    EEPROM.end();
    if (pdef < 0 || pdef >= NUM_PERFILES) pdef = 0;
    _perfilSeleccionado = pdef;
    _modoPerfilActivo = true;
    _lastBlink = millis();
    _blinkState = false;
}

bool estaModoPerfilActivo()
{
    return _modoPerfilActivo;
}

int perfilSeleccionado()
{
    return _perfilSeleccionado;
}

void manejarModoPerfil()
{
    if (!_modoPerfilActivo) return;

    if (millis() - _lastBlink >= 500) {
        _lastBlink = millis();
        _blinkState = !_blinkState;
        for (int r = 0; r < NUM_RELES; r++) {
            digitalWrite(PINES_LEDS_RELES[r], _blinkState ? HIGH : LOW);
        }
        for (int i = 0; i < NUM_PULSADORES; i++) {
            digitalWrite(PINES_LEDS[i], (i == _perfilSeleccionado && _blinkState) ? HIGH : LOW);
        }
    }

    for (int i = 0; i < NUM_PULSADORES; i++) {
        if (digitalRead(PINES_PULSADORES[i]) == LOW) {
            delay(30);
            if (digitalRead(PINES_PULSADORES[i]) == LOW) {
                _perfilSeleccionado = i;
            }
        }
    }

    if (digitalRead(PIN_PERFIL) == LOW) {
        delay(50);
        if (digitalRead(PIN_PERFIL) == LOW) {
            EEPROM.begin(EEPROM_TOTAL + 16);
            EEPROM.write(ADDR_PERFIL_DEFECTO, _perfilSeleccionado);
            EEPROM.commit();
            EEPROM.end();

            cargarConfiguracion(_perfilSeleccionado);

            _modoPerfilActivo = false;
            for (int r = 0; r < NUM_RELES; r++) digitalWrite(PINES_LEDS_RELES[r], LOW);
            for (int i = 0; i < NUM_PULSADORES; i++) digitalWrite(PINES_LEDS[i], LOW);
        }
    }
}

#endif
