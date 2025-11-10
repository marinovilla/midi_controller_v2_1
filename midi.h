#ifndef MIDI_H
#define MIDI_H  

#include <HardwareSerial.h>

void programa_midi(int programa) {
    Serial1.write(0xC0);
    Serial1.write(programa); 
    Serial.printf("Midi Ch: %d\n", programa);
}

void enviarExpresionMidi(int valor, int CC) {
    Serial1.write(0xB0);
    Serial1.write(CC);  
    Serial1.write(valor); 
    Serial.printf("CC%d= %d\n", CC, valor); 
}

#endif
