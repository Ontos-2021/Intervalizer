// audio.h
// =============================================================================
// Generación de tonos NO bloqueante usando millis(). tone() por defecto
// consume Timer2, lo que apaga PWM en pines 3 y 11; por eso LED_B se reubicó.
// =============================================================================
#ifndef INTERVALIZER_AUDIO_H
#define INTERVALIZER_AUDIO_H

#include <Arduino.h>
#include "config.h"

class AudioModule {
 public:
  static void begin();

  // Lanza un tono durante `duracion_ms` milisegundos.
  static void playTone(uint16_t freq, uint16_t duracion_ms);

  // Secuencia melódica: nota1 seguida de nota2. Si `gap_ms` > 0 se inserta
  // silencio entre ambas. Si `harmonico`, ambas suenan a la vez.
  static void playSecuencia(uint16_t f1, uint16_t f2,
                            uint16_t nota_ms, uint16_t gap_ms,
                            bool armonico);

  // Llamar en cada iteración del loop.
  static void update();

  static bool isPlaying();

 private:
  static uint16_t freq_;
  static uint32_t fin_;
  static bool     playing_;
};

#endif  // INTERVALIZER_AUDIO_H
