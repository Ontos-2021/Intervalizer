// eeprom_store.cpp
#include "eeprom_store.h"
#include <EEPROM.h>

bool EepromStore::inicializada_ = false;

uint8_t EepromStore::readU8(uint16_t a) {
  return EEPROM.read(a);
}

void EepromStore::writeU8(uint16_t a, uint8_t v) {
  EEPROM.update(a, v);  // update no escribe si el valor es igual (alarga vida EEPROM)
}

uint16_t EepromStore::readU16(uint16_t a) {
  return (uint16_t)readU8(a) | ((uint16_t)readU8(a + 1) << 8);
}

void EepromStore::writeU16(uint16_t a, uint16_t v) {
  writeU8(a,     (uint8_t)(v & 0xFF));
  writeU8(a + 1, (uint8_t)(v >> 8));
}

void EepromStore::begin() {
  if (readU8(EEPROM_OFFSET_MAGIC) == EEPROM_MAGIC) {
    inicializada_ = true;
    return;
  }
  // EEPROM virgen: inicializamos.
  writeU8(EEPROM_OFFSET_MAGIC, EEPROM_MAGIC);
  writeU8(EEPROM_OFFSET_NIVEL, 1);        // nivel 1 desbloqueado
  writeU16(EEPROM_OFFSET_HISC, 0);
  for (uint8_t i = 0; i < 12; ++i) {
    writeU8(EEPROM_OFFSET_OK + i, 0);    // correctas por intervalo
    writeU8(EEPROM_OFFSET_TOT + i, 0);   // total por intervalo
  }
  writeU16(EEPROM_OFFSET_G_OK, 0);
  writeU16(EEPROM_OFFSET_G_TOT, 0);
  inicializada_ = true;
}

uint8_t EepromStore::nivelDesbloqueado() {
  uint8_t n = readU8(EEPROM_OFFSET_NIVEL);
  if (n < 1) n = 1;
  if (n > TOTAL_NIVELES) n = TOTAL_NIVELES;
  return n;
}

void EepromStore::setNivelDesbloqueado(uint8_t n) {
  if (n < 1) n = 1;
  if (n > TOTAL_NIVELES) n = TOTAL_NIVELES;
  writeU8(EEPROM_OFFSET_NIVEL, n);
}

uint16_t EepromStore::highScore() {
  return readU16(EEPROM_OFFSET_HISC);
}

void EepromStore::setHighScore(uint16_t v) {
  writeU16(EEPROM_OFFSET_HISC, v);
}

uint8_t EepromStore::accuracy(uint8_t s) {
  if (s < 1) s = 1;
  if (s > 12) s = 12;
  uint8_t tot = readU8(EEPROM_OFFSET_TOT + (s - 1));
  if (tot == 0) return 255;            // sin datos
  uint8_t ok  = readU8(EEPROM_OFFSET_OK + (s - 1));
  uint16_t acc = (uint16_t)ok * 100u / (uint16_t)tot;
  if (acc > 100) acc = 100;
  return (uint8_t)acc;
}

void EepromStore::llenarAccuracy(uint8_t out[12]) {
  for (uint8_t s = 1; s <= 12; ++s) out[s - 1] = accuracy(s);
}

void EepromStore::registrarRespuesta(uint8_t s, bool correcta) {
  if (s < 1) s = 1;
  if (s > 12) s = 12;
  uint16_t aOk  = EEPROM_OFFSET_OK  + (s - 1);
  uint16_t aTot = EEPROM_OFFSET_TOT + (s - 1);
  uint8_t ok  = readU8(aOk);
  uint8_t tot = readU8(aTot);
  if (correcta && ok  < 255) ++ok;
  if (tot < 255) ++tot;
  writeU8(aOk,  ok);
  writeU8(aTot, tot);

  // Totales globales (saturan a 65535).
  uint16_t g_ok  = readU16(EEPROM_OFFSET_G_OK);
  uint16_t g_tot = readU16(EEPROM_OFFSET_G_TOT);
  if (correcta && g_ok  < 65535) ++g_ok;
  if (g_tot < 65535) ++g_tot;
  writeU16(EEPROM_OFFSET_G_OK,  g_ok);
  writeU16(EEPROM_OFFSET_G_TOT, g_tot);
}

uint16_t EepromStore::totalCorrectas() { return readU16(EEPROM_OFFSET_G_OK); }
uint16_t EepromStore::totalPreguntas() { return readU16(EEPROM_OFFSET_G_TOT); }

void EepromStore::setTotales(uint16_t ok, uint16_t tot) {
  writeU16(EEPROM_OFFSET_G_OK,  ok);
  writeU16(EEPROM_OFFSET_G_TOT, tot);
}

void EepromStore::reset() {
  writeU8(EEPROM_OFFSET_MAGIC, 0);  // forzar reinit en próxima begin()
  begin();
}