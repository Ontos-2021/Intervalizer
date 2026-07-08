// intervals.h
// =============================================================================
// Modelo musical: notas, intervalos, nombres y niveles.
// - notas_musicales: 25 notas cromáticas de A4 (440 Hz) a A6 (1760 Hz).
// - 12 intervalos (1..12 semitonos) más unísono (0) que se omite en juego.
// - Direcciones: ascendente (+), descendente (-), armónica (0) = ambas a la vez.
// - 5 niveles por grado de disonancia, alineados con la propuesta del README.
// =============================================================================
#ifndef INTERVALIZER_INTERVALS_H
#define INTERVALIZER_INTERVALS_H

#include <Arduino.h>

#define NOTA_LA4 0  // índice de A4 (440 Hz) dentro de notas_musicales

// Direcciones en que se puede emitir un intervalo
enum Direccion : int8_t {
  DIR_DESCENDENTE = -1,
  DIR_ASCENDENTE  = +1,
  DIR_HARMONICA   =  0,  // ambas notas suenan a la vez
};

// Resultado de elegir un intervalo en el motor de juego
struct Eleccion {
  uint8_t  semitonos;     // magnitud 1..12
  int8_t   direccion;     // -1, 0, +1
  uint8_t  indice_raiz;   // índice dentro de notas_musicales de la primera nota
};

class IntervalsModule {
 public:
  // Devuelve el nombre legible de un intervalo (1..12) en español corto.
  // 1->"2m", 2->"2M", 3->"3m", 4->"3M", 5->"4J", 6->"TT",
  // 7->"5J", 8->"6m", 9->"6M", 10->"7m", 11->"7M", 12->"8P".
  static const char* nombre(uint8_t semitonos);

  // Devuelve el número de intervalos habilitados en un nivel (1..12).
  static uint8_t cantidadEnNivel(uint8_t nivel);

  // Carga en `out` (capacidad >= 12) los intervalos del nivel.
  // Devuelve cuántos escribió.
  static uint8_t intervalosDeNivel(uint8_t nivel, uint8_t* out);

  // Selecciona un intervalo al azar dentro del nivel, ponderado por SRS.
  // `srs_acc[12]` contiene accuracy 0..100; un valor 255 = "sin datos".
  // Devuelve un semitono en 1..12.
  static uint8_t elegirIntervaloPonderado(uint8_t nivel,
                                          const uint8_t* srs_acc);

  // Elige una raíz y dirección que，保证 que la segunda nota cae dentro
  // del arreglo de notas. Aplica restricciones del intervalo.
  static Eleccion elegirRaizYDireccion(uint8_t semitonos, int8_t direccion);

  // Frecuencia (Hz) de la nota en el índice dado.
  static double nota(uint8_t indice);

  // Tamaño del arreglo de notas.
  static constexpr uint8_t notasCount() { return 25; }
};

#endif  // INTERVALIZER_INTERVALS_H
