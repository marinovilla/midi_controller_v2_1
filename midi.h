#ifndef MIDI_H
#define MIDI_H  

#include <HardwareSerial.h>

void programa_midi(int programa) {
    Serial_MIDI.write(0xC0);
    Serial_MIDI.write(programa); 
    Serial.printf("Midi Ch: %d\n", programa);
}

void enviarExpresionMidi(int valor, int CC) {
    Serial_MIDI.write(0xB0);
    Serial_MIDI.write(CC);  
    Serial_MIDI.write(valor); 
    Serial.printf("CC%d= %d\n", CC, valor); 
}

#endif
