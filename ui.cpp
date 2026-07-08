// ui.cpp
#include "ui.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 oled_(OLED_W, OLED_H, &Wire, OLED_RESET_VAL);

void UiModule::begin() {
  // Pin de reset del OLED reubicado en A1 (antes 4, chocaba con BOTON_2).
  pinMode(PIN_OLED_RST, OUTPUT);
  digitalWrite(PIN_OLED_RST, HIGH);
  Wire.begin();
  oled_.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  oled_.clearDisplay();
  oled_.display();
}

void UiModule::limpiar_() { oled_.clearDisplay(); }

void UiModule::texto_(uint8_t x, uint8_t y, uint8_t tam,
                      const char* s, bool inv) {
  oled_.setCursor(x, y);
  oled_.setTextSize(tam);
  if (inv) {
    oled_.setTextColor(BLACK, WHITE);
  } else {
    oled_.setTextColor(WHITE);
  }
  oled_.print(s);
  // Restaurar color por defecto para siguientes llamadas.
  oled_.setTextColor(WHITE);
}

void UiModule::dibujarBienvenida() {
  limpiar_();
  texto_(0, 0,  2, "Bienvenido");
  texto_(53, 20, 2, "a");
  texto_(6, 44, 2, "Intervalizer");
  oled_.display();
  delay(2200);

  limpiar_();
  texto_(0, 0, 2, "Entrena");
  texto_(28, 22, 2, "tu oido");
  oled_.display();
  delay(1600);
}

void UiModule::dibujarMenu(uint8_t res, uint8_t nivel_actual,
                           uint16_t high_score) {
  limpiar_();
  // Encabezado
  texto_(0, 0, 1, "Intervalizer  v0.2");
  // Línea separadora
  oled_.drawFastHLine(0, 9, 128, WHITE);

  // Opciones del menú
  const char* opciones[] = { "> Jugar", "> Nivel:", "> Stats" };
  uint8_t y = 14;
  for (uint8_t i = 0; i < 3; ++i) {
    bool inv = (i == res);
    texto_(0, y, 1, opciones[i], inv);
    if (i == 1) {  // Nivel: muestra el nivel actual
      char buf[12];
      sprintf(buf, " %u", nivel_actual);
      texto_(56, y, 1, buf, inv);
    }
    y += 12;
  }

  // Pie con high score
  char pie[24];
  sprintf(pie, "Record: %u", high_score);
  texto_(0, 54, 1, pie);

  oled_.display();
}

void UiModule::dibujarOpciones(const char* n1, const char* n2,
                               const char* n3, const char* n4,
                               char dir,
                               uint8_t ronda, uint8_t total,
                               uint8_t vidas, uint16_t score,
                               uint8_t nivel) {
  limpiar_();
  // Encabezado: Nivel Ronda/Total Vidas  Dir  Pts:score
  char h[28];
  sprintf(h, "N%u %u/%u V%u %c", nivel, ronda + 1, total, vidas, dir);
  texto_(0, 0, 1, h);
  texto_(92, 0, 1, "P:");
  char sc[8];
  sprintf(sc, "%u", score);
  texto_(100, 0, 1, sc);
  oled_.drawFastHLine(0, 9, 128, WHITE);

  // Cuatro opciones en grilla 2x2. Cada una: "<n>:<nombre>"
  const char* ns[4] = { n1, n2, n3, n4 };
  uint8_t xs[4] = { 2, 66, 2, 66 };
  uint8_t ys[4] = { 16, 16, 40, 40 };
  char buf[8];
  for (uint8_t i = 0; i < 4; ++i) {
    sprintf(buf, "%u:%s", i + 1, ns[i]);
    texto_(xs[i], ys[i], 2, buf);
  }

  oled_.display();
}

void UiModule::dibujarResultado(bool correcto, const char* nombre,
                                uint8_t vidas, uint16_t score) {
  limpiar_();
  if (correcto) {
    texto_(0, 0, 2, "Correcto!");
  } else {
    texto_(0, 0, 2, "Incorrecto");
  }
  char buf[20];
  sprintf(buf, "Era: %s", nombre);
  texto_(0, 26, 1, buf);
  sprintf(buf, "Vidas: %u  Pts: %u", vidas, score);
  texto_(0, 42, 1, buf);
  oled_.display();
}

void UiModule::dibujarNivelCompletado(uint8_t nivel, uint16_t score) {
  limpiar_();
  char buf[24];
  sprintf(buf, "Nivel %u OK", nivel);
  texto_(0, 0, 2, buf);
  sprintf(buf, "Puntos: %u", score);
  texto_(0, 24, 1, buf);
  texto_(0, 44, 1, "Btn: continuar");
  oled_.display();
}

void UiModule::dibujarStats(const uint8_t acc[12], uint16_t g_ok,
                            uint16_t g_tot, uint16_t high_score,
                            uint8_t nivel_desbl) {
  limpiar_();
  texto_(0, 0, 1, "Stats");
  oled_.drawFastHLine(0, 9, 128, WHITE);

  // Dos columnas: 6 intervalos a la izquierda (1..6) y 6 a la derecha (7..12).
  // nombres cortos para no consumir SRAM: los generamos del modulo intervals.
  // Aquí los escribimos hardcoded para no acoplar ui con intervals (mantén SRAM).
  static const char* nombres[12] = {
    "2m","2M","3m","3M","4J","TT","5J","6m","6M","7m","7M","8P"
  };
  char buf[10];
  for (uint8_t i = 0; i < 6; ++i) {
    if (acc[i] == 255) sprintf(buf, "%s: --", nombres[i]);
    else               sprintf(buf, "%s:%3u%%", nombres[i], acc[i]);
    texto_(0, 12 + i * 8, 1, buf);

    uint8_t j = i + 6;
    if (acc[j] == 255) sprintf(buf, "%s: --", nombres[j]);
    else               sprintf(buf, "%s:%3u%%", nombres[j], acc[j]);
    texto_(66, 12 + i * 8, 1, buf);
  }

  sprintf(buf, "Glob: %u/%u", g_ok, g_tot);
  texto_(0, 56, 1, buf);
  oled_.display();
}

void UiModule::dibujarGameOver(uint16_t score, uint16_t high_score) {
  limpiar_();
  texto_(0, 0, 2, "Game Over");
  char buf[24];
  sprintf(buf, "Puntos: %u", score);
  texto_(0, 24, 1, buf);
  bool record = (score >= high_score && score > 0);
  if (record) {
    texto_(0, 38, 1, "NUEVO RECORD!");
  } else {
    sprintf(buf, "Record: %u", high_score);
    texto_(0, 38, 1, buf);
  }
  texto_(0, 54, 1, "Btn: menu");
  oled_.display();
}