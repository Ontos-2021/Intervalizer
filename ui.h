// ui.h
// =============================================================================
// Capa de presentación sobre el OLED SSD1306 (128x64, I2C).
// Centraliza todos los dibujos (menú, opciones de quiz, resultado, stats,
// nivel completado) para que game.cpp no toque Adafruit_GFX directamente.
// =============================================================================
#ifndef INTERVALIZER_UI_H
#define INTERVALIZER_UI_H

#include <Arduino.h>
#include "config.h"

enum class Pantalla : uint8_t {
  None,
  Bienvenida,
  Menu,
  Opciones,
  ResultadoCorrecto,
  ResultadoIncorrecto,
  NivelCompletado,
  Stats,
  GameOver,
};

class UiModule {
 public:
  static void begin();

  // Pantalla de bienvenida (usada una vez en boot).
  static void dibujarBienvenida();

  // Menú principal. opcion resaltada = 0..2 (JUGAR / NIVEL / STATS).
  // 0..(MAX-1). Usamos 3 entradas.
  static void dibujarMenu(uint8_t resaltado, uint8_t nivel_actual,
                          uint16_t high_score);

  // Muestra las 4 opciones con el nombre del intervalo (no el número crudo).
  // `dir` = 'A' (ascendente) o 'D' (descendente); se muestra en el header.
  static void dibujarOpciones(const char* n1, const char* n2,
                               const char* n3, const char* n4,
                               char dir,
                               uint8_t ronda, uint8_t total_rondas,
                               uint8_t vidas, uint16_t score,
                               uint8_t nivel);

  // Resultado de una ronda.
  static void dibujarResultado(bool correcto, const char* intervalo_nombre,
                               uint8_t vidas, uint16_t score);

  // Pantalla de nivel completado.
  static void dibujarNivelCompletado(uint8_t nivel, uint16_t score);

  // Stats: accuracy por intervalo (12 valores 0..100 o 255).
  static void dibujarStats(const uint8_t acc[12], uint16_t global_ok,
                           uint16_t global_tot, uint16_t high_score,
                           uint8_t nivel_desbloqueado);

  // Game over (perdiste todas las vidas).
  static void dibujarGameOver(uint16_t score, uint16_t high_score);

 private:
  // Helpers
  static void limpiar_();
  static void texto_(uint8_t x, uint8_t y, uint8_t tam,
                     const char* s, bool invertido = false);
};

#endif  // INTERVALIZER_UI_H