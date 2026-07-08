// =============================================================================
// Intervalizer  v0.2  —  Entrenador físico de oído musical
// -----------------------------------------------------------------------------
// Sketch principal. La lógica vive en módulos (config/intervals/input/audio/
// ui/eeprom_store/game) para mantener el archivo legible y testeable.
//
// Convención de botones (INPUT_PULLUP):
//   MENU:   1=arriba  2=abajo  3=accion/entrar  4=accion secundaria
//   JUEGO:  1..4 = elegir opción 1..4
//   STATS / RESULTADO / NIVEL_OK / GAME_OVER:  cualquier botón avanza
//
// Pinout (ver config.h): Buzzer 9, Botones 2/4/7/8, LED RGB 6/5/12, OLED I2C.
// =============================================================================
#include <Wire.h>
#include "config.h"
#include "game.h"

void setup() {
  Serial.begin(9600);
  GameModule::begin();
}

void loop() {
  GameModule::tick();
}