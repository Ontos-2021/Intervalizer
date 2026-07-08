// game.h
// =============================================================================
// Máquina de estados finita (FSM) que orquesta el juego.
// Desacopla la lógica de juego del hardware (audio/UI/input) y de la
// persistencia (EEPROM). loop() solo llama a GameModule::tick().
//
// Estados:
//   BOOT            -> saludo inicial, luego MENU
//   MENU            -> navegar (JUGAR / NIVEL / STATS)
//   PLAYING         -> sub-fase GENERAR -> EMITIR -> ESPERAR -> RESULTADO
//   NIVEL_COMPLETADO-> pelicula corta, vuelve a MENU
//   GAME_OVER       -> muestra puntaje, vuelve a MENU
//   STATS           -> muestra accuracy, vuelve a MENU
// =============================================================================
#ifndef INTERVALIZER_GAME_H
#define INTERVALIZER_GAME_H

#include <Arduino.h>
#include "config.h"

class GameModule {
 public:
  static void begin();
  static void tick();   // llamada desde loop()

 private:
  enum Estado : uint8_t {
    ST_BOOT,
    ST_MENU,
    ST_STATS,
    ST_JUGAR,
    ST_NIVEL_COMPLETADO,
    ST_GAME_OVER,
  };
  enum Fase : uint8_t {
    FASE_GENERAR,
    FASE_EMITIR,
    FASE_ESPERAR,
    FASE_RESULTADO,
  };

  static void entrarMenu_();
  static void tickMenu_();
  static void entrarJugar_();
  static void tickJugar_();
  static void entrarNivelCompletado_();
  static void entrarGameOver_();
  static void entrarStats_();
  static void tickStats_();

  // Genera una nueva ronda: intervalo, dirección, opciones, reproduce.
  static void generarRonda_();
  static void esperarRespuesta_();
  static void mostrarResultado_();

  // Estado global
  static Estado estado_;
  static Fase   fase_;

  // Sesión de juego
  static uint8_t  nivel_;            // nivel actual (1..TOTAL_NIVELES)
  static uint8_t  ronda_;            // 0..RONDAS_POR_NIVEL-1
  static uint8_t  vidas_;
  static uint16_t score_;
  static uint8_t  aciertosNivel_;    // cuenta para avanzar de nivel

  // Ronda actual
  static uint8_t  semitonoCorrecto_;
  static int8_t   direccion_;
  static uint8_t  opcionesSemi_[4];
  static uint8_t  idxCorrecto_;      // 0..3 posición del botón correcto
  static bool     respondida_;
  static bool     fueCorrecta_;

  // Menú
  static uint8_t  menuSel_;          // 0=JUGAR 1=NIVEL 2=STATS
  static uint16_t ultimoDebounceBtn_;
};

#endif  // INTERVALIZER_GAME_H