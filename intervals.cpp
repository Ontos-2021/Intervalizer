// intervals.cpp
#include "intervals.h"
#include "config.h"

// 25 notas cromáticas, A4 (440 Hz) a A6 (1760 Hz).
// dos octavas completas más la nota final.
static const double kNotas[25] = {
  440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46,
  739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66,
  1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00
};

// Nombres cortos de los 12 intervalos (semitonos 1..12).
static const char* kNombres[13] = {
  "1m",  // 1  -> 2a menor (usamos 1m para uniformidad)
  "2m",  // 2
  "2M",  // 3
  "3m",  // 4
  "3M",  // 5
  "4J",  // 6
  "TT",  // 7  (cuarta aumentada / tritono)
  "5J",  // 8
  "6m",  // 9
  "6M",  // 10
  "7m",  // 11
  "7M",  // 12
  "8P",  // 13 -> octava
};

// Definición de los 5 niveles como máscaras de bits (bit i-1 = intervalo i).
// Bit i = semitono (i+1). Ej.: bit 3 = semitono 4 = 3a Mayor.
// Los niveles añaden intervalos de menor a mayor disonancia, igual que el README.
static const uint16_t kMascarasNivel[TOTAL_NIVELES] = {
  // Nivel 1: 3M, 4J, 5J            (semitonos 4,5,7  -> bits 3,4,6)
  (1u << 3) | (1u << 4) | (1u << 6),
  // Nivel 2: + 3m, 6M              (semitonos 3,9    -> bits 2,8)
  (1u << 3) | (1u << 4) | (1u << 6) | (1u << 2) | (1u << 8),
  // Nivel 3: + 2M, 7M              (semitonos 2,11   -> bits 1,10)
  (1u << 3) | (1u << 4) | (1u << 6) | (1u << 2) | (1u << 8) | (1u << 1) | (1u << 10),
  // Nivel 4: + 2m, TT, 7m          (semitonos 1,6,10 -> bits 0,5,9)
  (1u << 3) | (1u << 4) | (1u << 6) | (1u << 2) | (1u << 8)
            | (1u << 1) | (1u << 10) | (1u << 0) | (1u << 5) | (1u << 9),
  // Nivel 5: + 6m, 8P              (semitonos 8,12   -> bits 7,11) = los 12
  0x0FFF,
};

const char* IntervalsModule::nombre(uint8_t s) {
  if (s < 1) s = 1;
  if (s > 12) s = 12;
  return kNombres[s];
}

double IntervalsModule::nota(uint8_t indice) {
  if (indice >= notasCount()) indice = notasCount() - 1;
  return kNotas[indice];
}

// Cantidad de bits activos en una máscara de 12 bits.
static uint8_t popcount12(uint16_t m) {
  uint8_t c = 0;
  while (m) { c += (m & 1u); m >>= 1; }
  return c;
}

uint8_t IntervalsModule::cantidadEnNivel(uint8_t nivel) {
  if (nivel == 0) nivel = 1;
  if (nivel > TOTAL_NIVELES) nivel = TOTAL_NIVELES;
  return popcount12(kMascarasNivel[nivel - 1]);
}

uint8_t IntervalsModule::intervalosDeNivel(uint8_t nivel, uint8_t* out) {
  if (nivel == 0) nivel = 1;
  if (nivel > TOTAL_NIVELES) nivel = TOTAL_NIVELES;
  uint16_t m = kMascarasNivel[nivel - 1];
  uint8_t n = 0;
  for (uint8_t i = 0; i < 12; ++i) {
    if (m & (1u << i)) {
      out[n++] = i + 1;  // bit 0 = semitono 1
    }
  }
  return n;
}

// Selección ponderada por "necesidad": a menor accuracy, mayor probabilidad.
// Para evitar división por cero y para que intervalos aún no vistos
// (acc == 255) no dominen en exceso, se aplica un suavizado:
// peso = (101 - acc_clipped), con acc_clipped = min(acc, 100).
// Si acc == 255 (sin datos), se trata como 50.
uint8_t IntervalsModule::elegirIntervaloPonderado(uint8_t nivel,
                                                  const uint8_t* srs_acc) {
  uint8_t intervalos[12];
  uint8_t n = intervalosDeNivel(nivel, intervalos);

  uint32_t pesos[12];
  uint32_t total = 0;
  for (uint8_t i = 0; i < n; ++i) {
    uint8_t s = intervalos[i];
    uint8_t acc = srs_acc[s - 1];
    uint16_t w;
    if (acc == 255) {
      w = 60;  // nuevos: peso medio
    } else {
      // a menor accuracy, mayor peso (más necesidad de repaso)
      w = (acc > 100) ? 1 : (101 - acc);
      if (w < 5) w = 5;  // piso mínimo para que no se ignore
    }
    pesos[i] = w;
    total += w;
  }

  if (total == 0) return intervalos[random(0, n)];

  uint32_t r = random(0, (long)total);
  uint32_t acc2 = 0;
  for (uint8_t i = 0; i < n; ++i) {
    acc2 += pesos[i];
    if (r < acc2) return intervalos[i];
  }
  return intervalos[n - 1];
}

Eleccion IntervalsModule::elegirRaizYDireccion(uint8_t semitonos, int8_t direccion) {
  Eleccion e;
  e.semitonos = semitonos;
  e.direccion = direccion;

  uint8_t maxIdx = notasCount() - 1;  // 24
  int16_t minRaiz, maxRaiz;

  if (direccion == DIR_ASCENDENTE) {
    minRaiz = 0;
    maxRaiz = (int16_t)maxIdx - (int16_t)semitonos;
  } else if (direccion == DIR_DESCENDENTE) {
    minRaiz = (int16_t)semitonos;
    maxRaiz = (int16_t)maxIdx;
  } else {  // armónica: cualquier raíz cuya segunda nota quepa
    minRaiz = (int16_t)semitonos;
    maxRaiz = (int16_t)maxIdx - (int16_t)semitonos;
    if (maxRaiz < minRaiz) maxRaiz = minRaiz;
  }

  if (maxRaiz < minRaiz) {
    // intervalo demasiado grande para el rango; forzamos dirección ascendente
    minRaiz = 0;
    maxRaiz = (int16_t)maxIdx - (int16_t)semitonos;
    e.direccion = DIR_ASCENDENTE;
    if (maxRaiz < 0) maxRaiz = 0;
  }

  e.indice_raiz = (uint8_t)random((long)minRaiz, (long)(maxRaiz + 1));
  return e;
}
