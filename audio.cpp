// audio.cpp
// Implementación no bloqueante de tonos con millis().
// tone() en Arduino UNO usa Timer2, lo que desactiva PWM en pines 3 y 11;
// por eso el LED azul se reubicó en el pin 12 (ver config.h).
#include "audio.h"
#include "config.h"

uint16_t AudioModule::freq_   = 0;
uint32_t AudioModule::fin_    = 0;
bool     AudioModule::playing_ = false;

void AudioModule::begin() {
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
  playing_ = false;
}

void AudioModule::playTone(uint16_t freq, uint16_t duracion_ms) {
  freq_   = freq;
  fin_    = millis() + (uint32_t)duracion_ms;
  playing_ = true;
  tone(PIN_BUZZER, freq);
}

void AudioModule::update() {
  if (playing_ && (millis() >= fin_)) {
    noTone(PIN_BUZZER);
    playing_ = false;
  }
}

bool AudioModule::isPlaying() {
  return playing_;
}

// Secuencia melódica simple: nota1, (silencio), nota2.
// Si `armonico`, ambas notas suenan simultaneamente (intervalo armónico).
// Esta función ES bloqueante por naturaleza (secuencia corta, <1 s).
// El tiempo muerto original (2.7 s) se elimina: el juego continúa al terminar.
void AudioModule::playSecuencia(uint16_t f1, uint16_t f2,
                                 uint16_t nota_ms, uint16_t gap_ms,
                                 bool armonico) {
  if (armonico) {
    // Para intervalos armónicos reales necesitaríamos dos buzzer; aquí
    // emulamos tocando nota1 corta y luego la pareja. Aceptable con 1 buzzer.
    playTone(f1, nota_ms);
    // esperamos fin no bloqueando input en una versión futura; por simplicidad:
    uint32_t fin = millis() + nota_ms;
    while (millis() < fin) { update(); }
    playTone(f2, nota_ms);
    fin = millis() + nota_ms;
    while (millis() < fin) { update(); }
    return;
  }

  // Melódico: nota1 -> silencio -> nota2
  playTone(f1, nota_ms);
  uint32_t fin = millis() + nota_ms;
  while (millis() < fin) { update(); }
  if (gap_ms > 0) {
    fin = millis() + gap_ms;
    while (millis() < fin) { /* silencio */ }
  }
  playTone(f2, nota_ms);
  fin = millis() + nota_ms;
  while (millis() < fin) { update(); }
}