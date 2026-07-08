// input.cpp
#include "input.h"

const uint8_t InputModule::kPines[4] = { PIN_BTN_1, PIN_BTN_2, PIN_BTN_3, PIN_BTN_4 };
const uint8_t InputModule::kDebounceMs = 25;

uint8_t  InputModule::estadoActual_[4]  = {1, 1, 1, 1};
uint8_t  InputModule::estadoPrevio_[4]  = {1, 1, 1, 1};
uint32_t InputModule::tUltimoCambio_[4] = {0, 0, 0, 0};
bool     InputModule::eventoPendiente_[4] = {false, false, false, false};

void InputModule::begin() {
  for (uint8_t i = 0; i < 4; ++i) {
    pinMode(kPines[i], INPUT_PULLUP);
    estadoActual_[i]  = digitalRead(kPines[i]);
    estadoPrevio_[i]  = estadoActual_[i];
    tUltimoCambio_[i] = millis();
    eventoPendiente_[i] = false;
  }
}

void InputModule::update() {
  uint32_t ahora = millis();
  for (uint8_t i = 0; i < 4; ++i) {
    uint8_t lectura = digitalRead(kPines[i]);
    if (lectura != estadoPrevio_[i]) {
      tUltimoCambio_[i] = ahora;
      estadoPrevio_[i] = lectura;
    }
    if ((ahora - tUltimoCambio_[i]) >= kDebounceMs
        && estadoActual_[i] != estadoPrevio_[i]) {
      estadoActual_[i] = estadoPrevio_[i];
      // Flanco de bajada = pulsación (porque usamos INPUT_PULLUP)
      if (estadoActual_[i] == LOW) {
        eventoPendiente_[i] = true;
      }
    }
  }
}

Btn InputModule::consumePress() {
  for (uint8_t i = 0; i < 4; ++i) {
    if (eventoPendiente_[i]) {
      eventoPendiente_[i] = false;
      return (Btn)(i + 1);
    }
  }
  return BTN_NONE;
}

Btn InputModule::waitPress(uint32_t timeout_ms) {
  uint32_t inicio = millis();
  while (millis() - inicio < timeout_ms) {
    update();
    Btn b = consumePress();
    if (b != BTN_NONE) return b;
  }
  return BTN_NONE;
}
