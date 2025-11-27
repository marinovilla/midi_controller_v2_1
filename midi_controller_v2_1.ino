#include "html.h"
#include "config.h"
#include "midi.h"
#include "ModoConfiguracion.h"
#include "ModoFuncionamiento.h"
#include <HardwareSerial.h>
#include "IniciarEEPROM.h"

unsigned long Tiempo = 0;
bool modoConfiguracion = false;

void selector_modo() {
    Tiempo = millis();
    while (millis() - Tiempo < 5000) {
        if (digitalRead(PIN_MODO) == LOW) {
            modoConfiguracion = true;
            break;
        }
    }
    if (modoConfiguracion) {
        //ini_EEPROM();
        Modo_Configuracion();
    }
}

void setup() {
    Serial.begin(115200);
    Serial_MIDI.begin(31250, SERIAL_8N1, RXD_PIN, TXD_PIN);
    pinMode(PIN_MODO, INPUT_PULLUP);
    selector_modo();
}

void loop() {
  Modo_Funcionamiento();
}
