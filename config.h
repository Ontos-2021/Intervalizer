// config.h
// =============================================================================
// Pines físicos, constantes globales y layout EEPROM.
// Los pines se eligen para evitar colisiones entre el buzzer, los botones,
// el OLED y el LED RGB. En particular:
//   - El buzzer usa Timer2 (pin 9), lo que bloquea PWM en pines 3 y 11.
//   - Por eso LED_B se mueve al pin 12 (digital, sin PWM pero no choca con tone).
//   - OLED_RESET sale del pin 4 (que era BOTON_2) y se reubica en A1.
// =============================================================================
#ifndef INTERVALIZER_CONFIG_H
#define INTERVALIZER_CONFIG_H

#include <Arduino.h>

// ---- Hardware ---------------------------------------------------------------
#define PIN_BUZZER   9
#define PIN_BTN_1    2
#define PIN_BTN_2    4
#define PIN_BTN_3    7
#define PIN_BTN_4    8
#define PIN_LED_R    6
#define PIN_LED_G    5
#define PIN_LED_B    12   // movido desde 3 para evitar conflicto con tone()
#define PIN_OLED_SDA A4   // I2C por hardware en UNO
#define PIN_OLED_SCL A5
#define PIN_OLED_RST A1   // antes 4; ahora A1 para no chocar con BOTON_2

// ---- Pantalla ---------------------------------------------------------------
#define OLED_ADDR     0x3C
#define OLED_W        128
#define OLED_H        64
#define OLED_RESET_VAL -1 // reasignamos en setup() vía pinMode, no usamos reset chip

// ---- Juego ------------------------------------------------------------------
#define TOTAL_NIVELES      5
#define RONDAS_POR_NIVEL   10
#define VIDAS_INICIALES    3

// 12 intervalos (semitonos 1..12). El unísono (0) se omite porque no genera
// una segunda nota audible distinta; puede añadirse luego como bonus.
#define MAX_INTERVALO      12

// ---- EEPROM -----------------------------------------------------------------
// Layout de 32 bytes. Litte-endian en AVR.
//   [0]      magic (0xA5) indica datos inicializados.
//   [1]      nivel más alto desbloqueado (0..5).
//   [2..3]   high score de la sesión (uint16_t little-endian).
//   [4..15]  respuestas correctas por intervalo (12 × uint8_t).
//   [16..27] total de preguntas por intervalo (12 × uint8_t).
//   [28..29] total correctas (uint16_t).
//   [30..31] total preguntas (uint16_t).
#define EEPROM_MAGIC        0xA5
#define EEPROM_OFFSET_MAGIC 0
#define EEPROM_OFFSET_NIVEL 1
#define EEPROM_OFFSET_HISC  2
#define EEPROM_OFFSET_OK    4
#define EEPROM_OFFSET_TOT   16
#define EEPROM_OFFSET_G_OK  28
#define EEPROM_OFFSET_G_TOT 30

#endif  // INTERVALIZER_CONFIG_H
