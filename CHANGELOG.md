# Changelog

Todos los cambios notables de este proyecto se documentan aquí.
El formato se basa en [Keep a Changelog](https://keepachangelog.com/es/1.1.0/),
y este proyecto adhiere a [Versionado Semántico](https://semver.org/lang/es/).

## [Unreleased]

### Added
- `Documentación/AUDITORIA_360.md`: registro de las dos auditorías (360 inicial
  técnica+producto+innovación+monetización, y re-evaluación estratégica del salto
  comercial/funcional) realizadas sobre el proyecto.
- Arquitectura modular del firmware: `config.h`, `intervals`, `input`, `audio`,
  `ui`, `eeprom_store`, `game` (FSM) — el sketch principal queda en 25 líneas.
- Máquina de estados finita para el juego: `BOOT → MENU → JUGAR → RESULTADO →
  NIVEL_COMPLETADO / GAME_OVER → MENU`, basada en `millis()` (no bloqueante).
- 5 niveles por grado de disonancia (N1: 3M/4J/5J → N5: 12 intervalos + dirección
  descendente), alineados con la propuesta del `Readme.md`.
- Sistema de puntuación por sesión (+10 por acierto) y 3 vidas.
- Pantalla de menú navegable: JUGAR / NIVEL / STATS.
- Pantalla de estadísticas con accuracy por intervalo.
- Persistencia en EEPROM: nivel desbloqueado, high score, conteo correctas/total
  por intervalo (con magic byte para detectar EEPROM virgen).
- Repetición espaciada (SRS) básica: el intervalo con peor accuracy aparece con
  mayor probabilidad (`IntervalsModule::elegirIntervaloPonderado`).
- Nombres legibles de intervalo en el OLED (`3M`, `5J`, `TT`, `8P`, ...) en
  reemplazo del número crudo de semitonos.
- Botones con `INPUT_PULLUP`, anti-rebote por software y detección de flanco.
- `audio.cpp` no bloqueante usando `millis()`; eliminado el `delay(2700)` mudo.
- Timeout de 15 s por pregunta (cuenta como fallo si no se responde).
- Soporte de compilación con PlatformIO (`platformio.ini`).

### Changed
- Reubicación de pines para evitar colisiones de hardware:
  - `LED_B`: pin 3 → pin 12 (evita conflicto con `tone()` que desactiva PWM
    en los pines 3 y 11).
  - Reset del OLED: pin 4 → `A1` (pin 4 es `BOTON_2`).

### Fixed
- Bug de lectura fuera de rango en `elegirSegundaNota`: ahora
  `IntervalsModule::elegirRaizYDireccion` acota la raíz según dirección e
  intervalo; la segunda nota nunca cae fuera del arreglo de 25 notas.
- Bug off-by-one en `random(0, 17)` (excluía el último intervalo); reemplazado
  por selección ponderada sobre los intervalos del nivel.

### Removed
- Lógica monolítica del `Intervalizer.ino` original (~200 líneas en un solo
  `loop()` sin estado de juego).

## [0.1.0] - 2022-01-29
- Prototipo inicial: sketch de un solo archivo con `loop()` infinito, buzzer
  pasivo, 4 botones, OLED y LED RGB.