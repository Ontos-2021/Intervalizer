// input.h
// =============================================================================
// Lectura de 4 botones físicos con debounce por software y detección de
// flanco. Se usa INPUT_PULLUP, así que el pin está HIGH en reposo y
// va a LOW cuando se presiona (lógica invertida respecto al código original).
// =============================================================================
#ifndef INTERVALIZER_INPUT_H
#define INTERVALIZER_INPUT_H

#include <Arduino.h>
#include "config.h"

enum Btn : uint8_t {
  BTN_NONE = 0,
  BTN_1 = 1,
  BTN_2 = 2,
  BTN_3 = 3,
  BTN_4 = 4,
};

class InputModule {
 public:
  static void begin();

  // Llamar en cada iteración del loop para muestrear.
  static void update();

  // Devuelve el botón cuya pulsación se confirmó desde la última consulta
  // (consume el evento). Si no hay nada, devuelve BTN_NONE.
  static Btn consumePress();

  // Versión bloqueante con timeout (ms). Devuelve BTN_NONE si expira.
  static Btn waitPress(uint32_t timeout_ms);

 private:
  static const uint8_t kPines[4];
  static const uint8_t kDebounceMs;
  static uint8_t  estadoActual_[4];   // 1 = presionado, 0 = suelto
  static uint8_t  estadoPrevio_[4];
  static uint32_t tUltimoCambio_[4];
  static bool     eventoPendiente_[4];
};

#endif  // INTERVALIZER_INPUT_H
