#include "config.h"
#include "midi.h"
#include "ModoConfiguracion.h"
#include "ModoFuncionamiento.h"
#include <HardwareSerial.h>

unsigned long Tiempo = millis();
bool modoConfiguracion = false;

void setup() {
    Serial.begin(115200);
    Serial_MIDI.begin(31250, SERIAL_8N1, RXD_PIN, TXD_PIN); 
    pinMode(PIN_MODO, INPUT_PULLUP);
    Serial.println("Esperando para determinar el modo...");
    selector_modo();
}

void selector_modo() {
    while (millis() - Tiempo < 5000) {
        if (digitalRead(PIN_MODO) == LOW) {
            modoConfiguracion = true;
            break;
        }
    }
    if (modoConfiguracion) {
        Serial.println("*** Modo Configuración ***");
        Modo_Configuracion();    
    }
}

void loop() {
  Serial.println("*** Modo Funcionamiento ***"); 
  Modo_Funcionamiento();
}
