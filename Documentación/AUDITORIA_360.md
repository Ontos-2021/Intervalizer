# Auditoría 360 — Intervalizer

Documento generado el 2026-07-08. Registra las dos auditorías realizadas sobre el proyecto Intervalizer (entrenador físico de oído musical basado en Arduino UNO): una auditoría 360 inicial (técnica, producto, innovación, monetización y plan de prioridades) y una re-evaluación estratégica del salto al «siguiente nivel» comercial y funcional, bajo la tesis de un hardware que compite por soberanía atencional frente al teléfono.

---

## 1. Auditoría 360 (inicial)

### Veredicto Ejecutivo
Madurez actual: Prototipo de juguete (pre-MVP). Intervalizer es un sketch de Arduino UNO de archivo único, ~200 líneas, sin modularización, sin gestión de estado de juego, sin tests, sin sistema de niveles, sin puntuación persistente y con bugs lógicos documentables en el propio código. La intención del producto (un dispositivo físico que entrena reconocimiento de intervalos como un juego) es sólida y tiene demanda real en el nicho del ear-training, pero la implementación actual es apenas una demostración jugable de un único acertijo: nunca pasa de «una ronda → otra ronda» sin avance. No es MVP porque le faltan las tres cosas que definen un juego: progresión, feedback acumulado y fin de partida. Es un prototipo de concepto de hardware.

### Evidence Snapshot
- **Stack y arquitectura**: Hardware Arduino UNO Rev3, buzzer pasivo (pin 9), 4 botones (pines 2/4/7/8), LED RGB (pines 3/5/6 PWM), OLED SSD1306 128x64 por I2C. Firmware: un solo archivo `Intervalizer.ino`. Documentación con BOM de Fritzing (2022), esquemático, PCB, prototipo y carcasa STL 3D en `Modelo/Intervalizer.stl`. No hay `platformio.ini`, no hay `.gitignore` relevante, no hay tests, no hay CI.
- **Señales que justifican el veredicto**: `loop()` ejecuta una ronda completa (generar, emitir, leer, verificar) y luego reinicia sin estado entre rondas → no hay juego, hay un quiz infinito sin score. `verificarRespuesta` muestra «Correcto/Incorrecto» y duerme 2s; nunca acumula puntos ni nivel. El README describe niveles por consonancia/disonancia y estadísticas de intervalos — ninguna de esas features existe en el código. Es aspiracional, no implementado. Proyecto estancado desde 2022 (timestamp del BOM), sin commits recientes.

### Technical Audit
**Riesgos de ingeniería — los críticos primero:**

1. **Bug de `randomSeed(analogRead(A0))`**: A0 flotando es correcto como semilla; el problema real está abajo.
2. **Bug de `elegirIntervalo` y out-of-bounds en `elegirSegundaNota`** (el más serio del repo): `intervalos[]` tiene 18 elementos, `random(0,17)` excluye 17 (off-by-one), y `notas_musicales[numero_random + intervalo]` puede generar índice 25 → lectura fuera del arreglo de 25 elementos. Comportamiento indefinido: silbidos basura o cuelgue del Arduino. Crash latente.
3. **`imprimirOpciones` imprime el número de semitonos, no el nombre del intervalo**. El usuario ve «Opción 4: 7» y debe adivinar que 7 = quinta justa. Injugable para un principiante. Falta mapeo semitonos→nombre.
4. **`leerBotones()` es bloqueante y sin anti-bounce ni detección de flanco**. Sin `INPUT_PULLUP` y sin resistencia explícita, los botones flotantes generan lecturas ruidosas; mantener presionado responde la misma ronda varias veces.
5. **`emitirIntervalos` tiene `delay(2700)` mudo** después del tono: 2.7s de pantalla congelada sin input. Dead time improductivo.
6. **RGB usa `analogWrite` en pines 3/5/6** (correcto, PWM), pero el azul nunca se usa → LED RGB desperdiciado al 33%.

**Production-readiness gaps**: No hay FSM de juego (nada separa MENU → JUGANDO → RESULTADO → SIGUIENTE_NIVEL), todo en `loop()` con `delay()`. No hay debouncing ni interrupciones. No hay persistencia (EEPROM/Flash) de high-score ni de stats. No hay gestión de energía. No hay manejo de errores del OLED.

**Code quality**: Magic numbers (`delay(2250)`, `random(9,17)` sin comentar que 9-17 ubica la primera nota en el medio del arreglo). Funciones con side-effects (mutan globales, imprimen a Serial, escriben al OLED): mezcla UI + lógica + hardware. Sin header files, sin clases, sin separación de capas. `Serial.print` de debug siempre activo.

**Seguridad/performance/fiabilidad**: `tone()` monopoliza Timer 2 → bloquea PWM en pines 3 y 11. `LED_B` está en pin 3 → el canal azul del RGB no funciona mientras suena el buzzer. Bug silencioso.

### Product Audit
- **Qué producto parece ser**: un entrenador de oído musical físico, tipo Tamagotchi para músicos: enciendes, juegas, aprendes. La carcasa 3D y el OLED indican intención de dispositivo portátil independiente (no app de celular).
- **A quién sirve**: estudiantes de música/conservatorio principiantes e intermedios; gente que ya usa apps (EarMaster, Tenuto, Perfect Ear) pero quiere alternativa sin pantalla de celular, táctil, dedicada; docentes que quieran un objeto de estímulo en clase.
- **Por qué le importaría a alguien**: soberanía atencional (no es una app en el mismo teléfono donde llegan las notificaciones), sensación de instrumento (botones físicos), e identidad física (objeto de escritorio).
- **Qué le falta para ser compelling**: progresión (niveles, desbloqueo, dificultad creciente); score y stats persistentes (el README lo promete y no existe); nombres legibles de intervalos en el OLED; modo práctica vs modo juego; y audio real de calidad (el buzzer pasivo suena mal: el producto vive o muere por su sonido).

### Innovation Angle (ranked por leverage)
1. **(Máximo) Sound quality leap: DFPlayer Mini + parlante real en vez de buzzer.** El sonido es 100% del valor del producto. Muestras de piano/voz/cuerdas grabadas elevan el dispositivo de «proyecto escolar» a «gadget serio». Costo marginal US$3. ROI masivo.
2. **Interval engine powered by a real musical model, not arrays hardcoded.** Generador que tome tónica base, octava y produzca cualquier intervalo (temperado o justo), en modos melódico ascendente/descendente/armónico, compuesto, acordes, cadencias. Escala el contenido exponencialmente sin tocar hardware.
3. **Spaced-repetition engine (SRS) para intervalos débiles.** El README ya intuía «estadísticas de intervalos fuertes/débiles». Llevarlo a un algoritmo Anki-like: el peor accuracy aparece con mayor probabilidad. EEPROM. Ninguna app física de ear training hace SRS en hardware: es el moat por el que alguien paga.
4. **Modo «dictado melódico» / juegos extendidos.** Dictado de 3-5 notas, reconocimiento de acordes (mayor/menor/disminuido/aumentado), escalas, cadencias. Mismo hardware, nuevo firmware → nuevo modo desbloqueable.
5. **(Leverage medio-bajo, posponer) Bluetooth para app companion.** HC-05/ESP32 → heatmap de intervalos e historial. Te aleja del thesis «sin pantalla de celular».

### Monetization Paths
- **Path A — D2C gadget físico** (mejor fit con el DNA): dispositivo ensamblado con carcasa 3D terminada, PVP US$35-55, canal Etsy/Shopify/TikTok/Instagram. Downside: logística física y soporte.
- **Path B — Kit educativo DIY** (leverage maker): PCB + componentes + STL descargable + guía de armado + firmware open-source, US$25 kit o US$10 solo PCB+STL. Canal Tindie/ferias maker/escuelas. Margen y lealtad altos.
- **Path C — Firmware premium / «Intervalizer Pro»** (margen 100%): core open-source, modos avanzados (dictado melódico, SRS avanzado, afinación justa, integración DAW vía MIDI) licenciados por serial; US$8-15 por desbloqueo. Requiere base instalada.
- **Path D — Licencia B2B a escuelas** (volumen, no margen): lote de 10-20 + dashboard docente vía BLE/CSV, US$400-800 el set + suscripción anual. Ciclo de venta largo; posponer hasta MVP consolidado.
- **Recomendación**: Path A + B en paralelo (mismo dispositivo, dos presentaciones); Path C cuando haya base instalada; Path D mucho después.

### Priority Plan
- **NOW (esta semana, US$0)**: repo git público + `platformio.ini` + `.gitignore`; fix bug out-of-bounds en `elegirSegundaNota` y off-by-one en `random(0,17)`; mover OLED_RESET del pin 4 (choca con BOTON_2) a otro libre; mapear semitonos → nombres de intervalo en `imprimirOpciones`; quitar el `delay(2700)` mudo y reemplazar por ventana de escucha no bloqueante.
- **NEXT (4-8 semanas, roadmap a MVP real)**: refactor como FSM (MENU/PRACTICE/QUIZ/RESULTADO/NIVEL_COMPLETADO) con `millis()`; sistema de niveles por consonancia (README ya lo especifica); score acumulado persistente en EEPROM; debouncing de botones con detección de flanco + `INPUT_PULLUP`; modularizar en `audio.cpp`, `ui.cpp`, `game.cpp` + `main.ino`; decisión binaria buzzer vs DFPlayer Mini (probar DFPlayer en branch paralela y migrar si suena bien).
- **LATER (3-6 meses, apuestas de escala)**: SRS engine en EEPROM; migrar UNO → ESP32 si se quiere BLE + más samples en SD; carcasa 3D productizada (snap-fits, USB-C, bahía LiPo + cargador); validación pedagógica con 5-10 estudiantes; store Etsy/Shopify + packaging.

### Brutal Truth
El motivo más probable de fracaso sin cambios: sigue siendo un quiz infinito sin progresión, sin score y con nombres de intervalo ilegibles, reproducido por un buzzer que suena mal. Nadie juega más de 3 rondas porque no hay recompensa, no hay avance y no hay sonido agradable. El hardware físico es exactamente lo que lo diferencia de apps gratuitas como Perfect Ear, y al mismo tiempo el firmware actual no entrega ni el feature más básico del README. El proyecto se congeló 4 años por una razón: el salto de «demo jugable» a «MVP de juego» requiere deconstruir `loop()` en un autómata de estados, y ese trabajo invisible no se hizo. Trampa clásica de un proyecto que se quedó en el «funciona una vez». El desbloqueo es barato: 1 fin de semana de refactor a FSM + 1 fin de semana de DFPlayer Mini + nombres legibles = MVP jugable.

---

## 2. Re-evaluación estratégica — salto al máximo nivel

### Objetivo
Convertir un prototipo de Arduino congelado en un **dispositivo físico de entrenamiento auditivo de marca**, que compita no por features sino por **soberanía atencional**: un objeto dedicado que gana justo porque no es el teléfono. La pregunta real no es «¿qué features le pongo?» sino «¿cómo convierto este hardware en una marca de la que alguien paga US$50 y la recomienda, y en un motor de contenido que genera ingreso recurrente sin volver a tocar la línea de ensamblaje?».

### Bottleneck
El cuello no es técnico, ni de BOM, ni de hardware. **Es de producto y de distribución.** El firmware actual no entrega el producto que el README describe, y no existe marca, canal ni narrativa de venta. Mientras el dispositivo sea «un quiz infinito con buzzer malo», ninguna campaña de marketing lo salva. Si se arregla el firmware y el sonido y se construye narrativa, el mismo hardware actual ya es vendible. El salto al «siguiente nivel» es 80% software+marca y 20% hardware — proporción contraria al instinto maker.

### Mercado validado (por qué la tesis es correcta)
La categoría «focus device» está en auge: Light Phone II (~US$300, US$5M+ en ventas), Daylight Computer (e-ink, US$800+), Pomodoro timers físicos, Yoto player. Existe audiencia dispuesta a pagar por un objeto que hace una sola cosa bien. Intervalizer encaja: «tu oído musical, sin que te llegue el WhatsApp». Esa frase de posicionamiento es más valiosa que cualquier feature.

### Highest-Leverage Moves (ranked)
1. **(Máximo, costo ~0) Convertir el firmware en el producto completo.** FSM (MENU/PRACTICA/QUIZ/RESULTADO/NIVEL), 5 niveles por consonancia, score + racha + EEPROM, nombres legibles («5J», «3M», «tritono»), SRS por intervalo, reemplazar `delay()` por `millis()`. Convierte el dispositivo en vendible sin tocar el BOM.
2. **(Alto, ~US$3 de BOM) Salto de audio: DFPlayer Mini + parlante real + microSD.** El buzzer pasivo es el único argumento en contra. Muestras de piano/guitarra/voz elevan de «proyecto escolar» a «gadget serio». ROI masivo.
3. **(Comercial máximo, costo ~0) Definir una marca y frase de posicionamiento.** No es «un entrenador de intervalos», es «el lugar físico donde entrenas tu oído sin pantallas». Logo, tono, paleta, tagline y fotos. Permite cobrar US$50 en vez de US$20.
4. **Contenido como motor de ingreso recurrente (la jugada de Kindle).** Hardware = ingreso único; firmware = recurrente sin fábrica. Firmware core open-source (comunidad). «Intervalizer Pro» unlock único (~US$12): SRS avanzado, dictado melódico, intervalos compuestos, modos jazz/atonal. Content packs (~US$5 c/u): pack Jazz, pack Solfeo clásico, pack Afinación justa, pack Dictado armónico. Licencias por serial en EEPROM (challenge-response simple).
5. **(Diferido) Migrar MCU a ESP32 a futuro, NO ahora.** Bluetooth para dashboard docente, más samples en SD, OTA. Cambiar de MCU solo cuando el UNO sea el cuello comprobado (que hoy no lo es). El cuello es el firmware, no el silicio.
6. **(Medio, post-MVP) Carcasa 3D productizada.** Snap-fits, USB-C, bahía para LiPo, DFM. Se hace después del firmware. B2B a escuelas se pospone hasta validar D2C y pedagogía.

### Execution Plan
- **NOW (esta semana, US$0)**: repo git + `platformio.ini` + `.gitignore`; fix bugs críticos; arrancar refactor a FSM + niveles + score + EEPROM; definir tagline + paleta (1 tarde).
- **NEXT (4-8 semanas, <US$20)**: integrar DFPlayer Mini + parlante + microSD y migrar motor de audio a samples; implementar SRS por intervalo y 5 niveles; carcasa 3D v2 con bahía para parlante; validación con 5-10 estudiantes (rondas/sesión, retención D1 vs D7, precisión intervalos antes/después); tienda Etsy/Shopify con fotos reales + video TikTok.
- **LATER (3-6 meses)**: Firmware Pro unlock + sistema de content packs; si UNO se queda corto → migrar a ESP32-C3; kit DIY para makers; B2B dashboard docente + lote piloto de 10 unidades.

### Métricas (los números que deben moverse)
- **Producto**: rondas/jugador > 8; retención D1 > 40 %, D7 > 20 %; precisión por intervalo mejora > 15 % en 20 min de uso.
- **Unidad**: media de minutos/sesión > 10 min.
- **Comercial D2C**: margen bruto > 55 % con PVP US$50; CAC orgánico < US$10; conversión visita→compra > 2 %.
- **Recurrente**: % de usuarios que compran al menos 1 pack Pro/de contenido en 90 días > 15 %.
- **Validación**: NPS en piloto > 40 (umbral de lanzamiento).
(Métricas de vanidad — «followers», «líneas de código» — se excluyen.)

### Hard Truth (tres verdades incómodas)
1. **El hardware es lo de menos en este salto.** El instinto maker dice «mejor PCB, mejor MCU, mejor carcasa». Falso: el cuello hoy es firmware incompleto + audio malo + falta de marca. 200 horas rediseñando PCB antes de tener FSM + SRS es optimizar lo que no es el cuello. El dispositivo actual, con firmware terminado y DFPlayer, ya se vende.
2. **El buzzer pasivo tiene que morir.** No hay versión del producto con buzzer que compita; suena como un despertador. Es una decisión, no una tendencia. Cada día que se añaden features sobre el buzzer se pule lo que se va a descartar.
3. **Si no se define marca y canal antes de que la funcionalidad esté lista, no se vende.** El 80 % de los proyectos de hardware físico mueren porque se construye lo «obvio» y luego no hay narrativa. La narrativa de Intervalizer es preciosa: «entrenar tu oído sin la interferencia de tu móvil». Esa frase paga el margen. Pero hay que decirla con fotos, video y un rostro humano. Pide ayuda con eso como parte del roadmap, no como «después».

### Próximos pasos tras la auditoría (estado: implementado en v0.2)
La fase NOW ya fue implementada y commiteada: arquitectura modular (config/intervals/input/audio/ui/eeprom_store/game), FSM basada en `millis()`, 5 niveles por consonancia, score + 3 vidas, high score y SRS en EEPROM, nombres legibles de intervalo, botones con `INPUT_PULLUP` + debounce, pines reubicados (LED_B→12, reset OLED→A1), build limpio en PlatformIO (RAM 52.6 %, Flash 73.6 %), `CHANGELOG.md` añadido. Quedan pendientes: DFPlayer Mini (jugada #2), marca+tagline+paleta (jugada #3) y content packs (jugada #4).

---

La siguiente fase recomendada es **DFPlayer Mini + parlante real** (audio de samples), por ser el mayor salto percibido de calidad y el más barato.

---