// eeprom_store.h
// =============================================================================
// Persistencia en EEPROM del ATmega328P (1 KB disponible). Guarda:
//   - magic byte (para detectar EEPROM virgen)
//   - nivel más alto desbloqueado
//   - high score de la mejor sesión
//   - cuenta de aciertos y total por intervalo (para SRS)
//   - totales globales (aciertos / preguntas)
// Todo se lee/escribe de a 1 byte o 2 bytes con cuidado del little-endian.
// =============================================================================
#ifndef INTERVALIZER_EEPROM_STORE_H
#define INTERVALIZER_EEPROM_STORE_H

#include <Arduino.h>
#include "config.h"

class EepromStore {
 public:
  // Carga datos; si la EEPROM está virgen (magic != 0xA5) la inicializa.
  static void begin();

  static uint8_t  nivelDesbloqueado();          // 0..TOTAL_NIVELES
  static void     setNivelDesbloqueado(uint8_t n);

  static uint16_t highScore();
  static void     setHighScore(uint16_t v);

  // SRS: accuracy 0..100 por intervalo (índice 0..11 = semitono 1..12).
  // 255 = sin datos (se trata como 50% en la selección).
  static uint8_t  accuracy(uint8_t semitono);
  static void     llenarAccuracy(uint8_t out[12]);

  // Registra una respuesta (correcta o no) para un semitono.
  static void     registrarRespuesta(uint8_t semitono, bool correcta);

  // Totales globales (uint16_t).
  static uint16_t totalCorrectas();
  static uint16_t totalPreguntas();
  static void     setTotales(uint16_t ok, uint16_t tot);

  // Resetea TODA la EEPROM a valores por defecto (cuidado: borra progreso).
  static void     reset();

 private:
  static uint8_t  readU8(uint16_t addr);
  static void     writeU8(uint16_t addr, uint8_t v);
  static uint16_t readU16(uint16_t addr);
  static void     writeU16(uint16_t addr, uint16_t v);
  static bool     inicializada_;
};

#endif  // INTERVALIZER_EEPROM_STORE_H