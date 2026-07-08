// game.cpp
#include "game.h"
#include "config.h"
#include "intervals.h"
#include "audio.h"
#include "ui.h"
#include "input.h"
#include "eeprom_store.h"

// ---- Definición de estáticos ------------------------------------------------
GameModule::Estado GameModule::estado_ = ST_BOOT;
GameModule::Fase   GameModule::fase_   = FASE_GENERAR;

uint8_t  GameModule::nivel_          = 1;
uint8_t  GameModule::ronda_          = 0;
uint8_t  GameModule::vidas_          = VIDAS_INICIALES;
uint16_t GameModule::score_          = 0;
uint8_t  GameModule::aciertosNivel_  = 0;

uint8_t  GameModule::semitonoCorrecto_ = 4;
int8_t   GameModule::direccion_        = DIR_ASCENDENTE;
uint8_t  GameModule::opcionesSemi_[4]  = {0,0,0,0};
uint8_t  GameModule::idxCorrecto_      = 0;
bool     GameModule::respondida_       = false;
bool     GameModule::fueCorrecta_      = false;

uint8_t  GameModule::menuSel_          = 0;
uint16_t GameModule::ultimoDebounceBtn_ = 0;

// ---- Helper: pausa corta entre transiciones (anti rebote visual) -----------
static inline void antiReboteBoton_(uint16_t ms = 200) {
  uint32_t fin = millis() + ms;
  while (millis() < fin) { InputModule::update(); }
}

// =============================================================================
void GameModule::begin() {
  EepromStore::begin();
  UiModule::begin();
  AudioModule::begin();
  InputModule::begin();

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  analogWrite(PIN_LED_R, 0);
  analogWrite(PIN_LED_G, 0);
  digitalWrite(PIN_LED_B, LOW);

  randomSeed(analogRead(A0) ^ micros());

  estado_ = ST_BOOT;
  UiModule::dibujarBienvenida();
  entrarMenu_();
}

// ---------------- MENÚ -------------------------------------------------------
void GameModule::entrarMenu_() {
  estado_ = ST_MENU;
  menuSel_ = 0;
  nivel_   = EepromStore::nivelDesbloqueado();
  if (nivel_ < 1) nivel_ = 1;
  UiModule::dibujarMenu(menuSel_, nivel_, EepromStore::highScore());
  antiReboteBoton_();
}

void GameModule::tickMenu_() {
  Btn b = InputModule::consumePress();
  if (b == BTN_NONE) return;

  switch (b) {
    case BTN_NONE: return;
    case BTN_1:  // subir cursor (cíclico)
      menuSel_ = (menuSel_ + 2) % 3;
      break;
    case BTN_2:  // bajar cursor
      menuSel_ = (menuSel_ + 1) % 3;
      break;
    case BTN_3:  // acción contextual
      if (menuSel_ == 0) {
        entrarJugar_();
        return;
      } else if (menuSel_ == 1) {
        // subir nivel dentro de los desbloqueados
        uint8_t maxN = EepromStore::nivelDesbloqueado();
        if (maxN < 1) maxN = 1;
        nivel_++;
        if (nivel_ > maxN) nivel_ = 1;
      } else {
        entrarStats_();
        return;
      }
      break;
    case BTN_4:  // en "NIVEL" baja el nivel; en "JUGAR" es atajo para jugar
      if (menuSel_ == 1) {
        if (nivel_ > 1) nivel_--;
      } else if (menuSel_ == 0) {
        entrarJugar_();
        return;
      }
      break;
  }
  UiModule::dibujarMenu(menuSel_, nivel_, EepromStore::highScore());
  antiReboteBoton_();
}

// ---------------- STATS ------------------------------------------------------
void GameModule::entrarStats_() {
  estado_ = ST_STATS;
  uint8_t acc[12];
  EepromStore::llenarAccuracy(acc);
  UiModule::dibujarStats(acc,
                         EepromStore::totalCorrectas(),
                         EepromStore::totalPreguntas(),
                         EepromStore::highScore(),
                         EepromStore::nivelDesbloqueado());
  antiReboteBoton_();
}

void GameModule::tickStats_() {
  if (InputModule::consumePress() != BTN_NONE) {
    entrarMenu_();
  }
}

// ---------------- JUGAR ------------------------------------------------------
void GameModule::entrarJugar_() {
  estado_ = ST_JUGAR;
  fase_   = FASE_GENERAR;
  ronda_  = 0;
  vidas_  = VIDAS_INICIALES;
  score_  = 0;
  aciertosNivel_ = 0;
  // La primera ronda la genera tick() en cuanto vea fase=FASE_GENERAR.
}

void GameModule::generarRonda_() {
  fase_ = FASE_GENERAR;

  // Dirección: niveles 1..4 ascendente; nivel 5 mezcla asc/desc.
  if (nivel_ >= TOTAL_NIVELES) {
    direccion_ = (random(0, 2) == 0) ? DIR_ASCENDENTE : DIR_DESCENDENTE;
  } else {
    direccion_ = DIR_ASCENDENTE;
  }

  // SRS: lista de accuracy 0..100 (255 = sin datos).
  uint8_t srs[12];
  EepromStore::llenarAccuracy(srs);
  semitonoCorrecto_ = IntervalsModule::elegirIntervaloPonderado(nivel_, srs);

  // Raíz válida para la dirección.
  Eleccion e = IntervalsModule::elegirRaizYDireccion(semitonoCorrecto_,
                                                    direccion_);

  // Distractores: 3 intervalos distintos del mismo nivel, != correcto.
  uint8_t listaNivel[12];
  uint8_t nNivel = IntervalsModule::intervalosDeNivel(nivel_, listaNivel);
  if (nNivel < 4) {
    // Si hay menos de 4 intervalos en el nivel (no debería pasar con L1=3? Sí: L1=3)
    // rellenamos con cualquier semitono 1..12 distinto.
    // Para L1 (3 intervalos) necesitamos 4 opciones → repetimos permitiendo
    // duplicados solo si es estrictamente necesario, pero mejor ampliamos
    // distractor pool al rango 1..12.
    nNivel = 0;
    for (uint8_t s = 1; s <= 12; ++s) {
      if (s != semitonoCorrecto_) listaNivel[nNivel++] = s;
    }
  }

  uint8_t distSemis[3];
  for (uint8_t k = 0; k < 3; ++k) {
    uint8_t cand;
    bool ok = false;
    uint8_t intentos = 0;
    do {
      cand = listaNivel[random(0, nNivel)];
      ok = (cand != semitonoCorrecto_);
      for (uint8_t j = 0; j < k && ok; ++j) ok = (cand != distSemis[j]);
      ++intentos;
    } while (!ok && intentos < 64);
    distSemis[k] = cand;
  }

  // Llenar opciones y barajar.
  opcionesSemi_[0] = semitonoCorrecto_;
  opcionesSemi_[1] = distSemis[0];
  opcionesSemi_[2] = distSemis[1];
  opcionesSemi_[3] = distSemis[2];
  for (uint8_t i = 3; i > 0; --i) {
    uint8_t j = (uint8_t)random(0, i + 1);
    uint8_t tmp = opcionesSemi_[i];
    opcionesSemi_[i] = opcionesSemi_[j];
    opcionesSemi_[j] = tmp;
  }
  // Encontrar posición del correcto.
  idxCorrecto_ = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    if (opcionesSemi_[i] == semitonoCorrecto_) { idxCorrecto_ = i; break; }
  }

  // Frecuencias.
  double f1 = IntervalsModule::nota(e.indice_raiz);
  double f2;
  if (direccion_ == DIR_ASCENDENTE)  f2 = IntervalsModule::nota(e.indice_raiz + semitonoCorrecto_);
  else                               f2 = IntervalsModule::nota(e.indice_raiz - semitonoCorrecto_);

  // Dibujar opciones.
  char dir = (direccion_ == DIR_ASCENDENTE) ? 'A' : 'D';
  UiModule::dibujarOpciones(
      IntervalsModule::nombre(opcionesSemi_[0]),
      IntervalsModule::nombre(opcionesSemi_[1]),
      IntervalsModule::nombre(opcionesSemi_[2]),
      IntervalsModule::nombre(opcionesSemi_[3]),
      dir, ronda_, RONDAS_POR_NIVEL, vidas_, score_, nivel_);

  // Emitir secuencia (bloqueante corto ~700ms).
  fase_ = FASE_EMITIR;
  analogWrite(PIN_LED_R, 200);
  AudioModule::playSecuencia((uint16_t)f1, (uint16_t)f2,
                             /*nota_ms*/ 350, /*gap_ms*/ 120,
                             /*armonico*/ false);
  analogWrite(PIN_LED_R, 0);

  fase_ = FASE_ESPERAR;
  respondida_ = false;
}

void GameModule::esperarRespuesta_() {
  // Timeout de 15 s: si no responde, cuenta como incorrecta.
  Btn b = InputModule::waitPress(15000);
  if (b == BTN_NONE) {
    // timeout: incorrecta
    fueCorrecta_ = false;
  } else {
    uint8_t eleccion = (uint8_t)b - 1;  // BTN_1..4 -> 0..3
    fueCorrecta_ = (eleccion == idxCorrecto_);
  }
  fase_ = FASE_RESULTADO;
  mostrarResultado_();
}

void GameModule::mostrarResultado_() {
  EepromStore::registrarRespuesta(semitonoCorrecto_, fueCorrecta_);

  if (fueCorrecta_) {
    score_ += 10;
    aciertosNivel_++;
    UiModule::dibujarResultado(true,
        IntervalsModule::nombre(semitonoCorrecto_), vidas_, score_);
    analogWrite(PIN_LED_G, 255);
  } else {
    if (vidas_ > 0) vidas_--;
    UiModule::dibujarResultado(false,
        IntervalsModule::nombre(semitonoCorrecto_), vidas_, score_);
    analogWrite(PIN_LED_R, 255);
  }

  // Esperar a que el usuario pulse para continuar (timeout largo = 4s auto).
  InputModule::waitPress(4000);

  // Apagar LEDs de feedback.
  analogWrite(PIN_LED_G, 0);
  analogWrite(PIN_LED_R, 0);

  // Avance de ronda / nivel / game over.
  ronda_++;
  if (vidas_ == 0) {
    entrarGameOver_();
    return;
  }
  if (ronda_ >= RONDAS_POR_NIVEL) {
    // Fin del nivel: ¿aprueba?
    // Criterio: al menos 60% de aciertos.
    if (aciertosNivel_ * 100u / RONDAS_POR_NIVEL >= 60u) {
      // Desbloquear siguiente nivel (si corresponde) y mostrar pantalla.
      uint8_t desbl = EepromStore::nivelDesbloqueado();
      if (nivel_ == desbl && nivel_ < TOTAL_NIVELES) {
        EepromStore::setNivelDesbloqueado(nivel_ + 1);
      }
      entrarNivelCompletado_();
    } else {
      // Repite el nivel (no avanza).
      entrarGameOver_();  // tratamos como fin de partida sin nuevo record
    }
    return;
  }

  generarRonda_();
}

void GameModule::entrarNivelCompletado_() {
  estado_ = ST_NIVEL_COMPLETADO;
  // Guardar high score si corresponde.
  if (score_ > EepromStore::highScore()) EepromStore::setHighScore(score_);
  UiModule::dibujarNivelCompletado(nivel_, score_);
  antiReboteBoton_();
  // Esperar un botón para volver al menú.
  while (InputModule::consumePress() == BTN_NONE) {
    InputModule::update();
  }
  entrarMenu_();
}

void GameModule::entrarGameOver_() {
  estado_ = ST_GAME_OVER;
  if (score_ > EepromStore::highScore()) EepromStore::setHighScore(score_);
  UiModule::dibujarGameOver(score_, EepromStore::highScore());
  antiReboteBoton_();
  while (InputModule::consumePress() == BTN_NONE) {
    InputModule::update();
  }
  entrarMenu_();
}

// ---------------- TICK principal ---------------------------------------------
void GameModule::tick() {
  InputModule::update();
  AudioModule::update();

  switch (estado_) {
    case ST_BOOT:           entrarMenu_();           break;
    case ST_MENU:           tickMenu_();             break;
    case ST_STATS:          tickStats_();            break;
    case ST_JUGAR:
      switch (fase_) {
        case FASE_GENERAR:  generarRonda_();         break;
        case FASE_EMITIR:                            break;
        case FASE_ESPERAR:  esperarRespuesta_();     break;
        case FASE_RESULTADO:                         break;
      }
      break;
    case ST_NIVEL_COMPLETADO: entrarMenu_();         break;
    case ST_GAME_OVER:        entrarMenu_();         break;
  }
}