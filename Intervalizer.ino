// Definimos variables para las entradas físicas del Arduino y sus respectivos componentes.
#define BUZZER 9
#define BOTON_1 2
#define BOTON_2 4
#define BOTON_3 7
#define BOTON_4 8
#define LED_R 6
#define LED_G 5
#define LED_B 3

// Declaramos variables para la pantalla OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dimensiones de la pantalla OLED
#define ANCHO 128
#define ALTO 64

// Instanciamos el objeto "oled" de la clase "Adafruit_SSD1306".
#define OLED_RESET 4
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);

// Array con las frecuencias de las notas musicales (dos octavas, de La4 a La6)
const double notas_musicales[] = {
  440, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46,
  739.99, 783.99, 830.61, 880, 932.33, 987.77, 1046.5, 1108.73, 1174.66,
  1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760
};

// Definimos los intervalos disponibles (3ra Mayor, 4ta Justa, 5ta Justa y 6ta mayor)
const int intervalos[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -2, -3, -4, -5, -6, -7, -8, -9};

void setup() {
  Serial.begin(9600); // Iniciamos la comunicación serial

  // Definimos los pines como entradas o salidas
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTON_1, INPUT);
  pinMode(BOTON_2, INPUT);
  pinMode(BOTON_3, INPUT);
  pinMode(BOTON_4, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // RandomSeed - Permite que la secuencia de números siempre sea distinta.
  randomSeed(analogRead(A0));

  // Iniciamos la pantalla OLED
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0X3C);

  // Bienvenida al juego
  saludar();
}

void loop() {
  // Elegimos la primera nota al azar
  int numero_random = random(9, 17);
  double primer_nota = elegirPrimerNota(numero_random);

  // Elegimos el intervalo al azar
  int intervalo = elegirIntervalo();

  // Definimos la segunda nota en base al intervalo
  double segunda_nota = elegirSegundaNota(intervalo, numero_random);

  Serial.println("-------");

  // Generamos las opciones posibles (una correcta y tres incorrectas)
  int opcion_correcta = definirIntervaloCorrecto(intervalo);
  int opcion_1, opcion_2, opcion_3;
  do {
    opcion_1 = generarIntervaloIncorrecto(intervalo);
  } while (opcion_1 == opcion_correcta);
  do {
    opcion_2 = generarIntervaloIncorrecto(intervalo);
  } while (opcion_2 == opcion_correcta || opcion_2 == opcion_1);
  do {
    opcion_3 = generarIntervaloIncorrecto(intervalo);
  } while (opcion_3 == opcion_correcta || opcion_3 == opcion_1 || opcion_3 == opcion_2);

  imprimirOpciones(opcion_correcta, opcion_1, opcion_2, opcion_3);
  emitirIntervalos(primer_nota, segunda_nota);

  // Esperar respuesta del usuario
  int respuesta_usuario = leerBotones();
  verificarRespuesta(respuesta_usuario, opcion_correcta);
}

void saludar() {
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(2);
  oled.println("Bienvenido");
  oled.setCursor(53, 18);
  oled.println("a");
  oled.setCursor(25, 40);
  oled.setTextSize(1, 2);
  oled.println("Intervalizer");

  oled.display();
  delay(2250);

  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(3, 0);
  oled.setTextSize(2);
  oled.println("Entrena tu");
  oled.setCursor(34, 20);
  oled.println("oido");
  oled.setCursor(18, 40);
  oled.println("musical");
  oled.display();
  delay(2000);
}

double elegirPrimerNota(int numero_random) {
  double primer_nota = notas_musicales[numero_random];
  Serial.print("Esta es la primer nota: ");
  Serial.println(primer_nota);
  return primer_nota;
}

int elegirIntervalo() {
  int intervalo = intervalos[random(0, 17)];
  Serial.print("Este es el intervalo: ");
  Serial.println(intervalo);
  return intervalo;
}

double elegirSegundaNota(int intervalo, int numero_random) {
  double segunda_nota = notas_musicales[numero_random + intervalo];
  Serial.print("Esta es la segunda nota: ");
  Serial.println(segunda_nota);
  return segunda_nota;
}

void emitirIntervalos(double primer_nota, double segunda_nota) {
  tone(BUZZER, primer_nota, 200);
  analogWrite(LED_R, 255);
  delay(400);
  analogWrite(LED_R, 0);

  tone(BUZZER, segunda_nota, 200);
  analogWrite(LED_G, 230);
  delay(400);
  analogWrite(LED_G, 0);

  delay(2700);
}

int definirIntervaloCorrecto(int intervalo) {
  if (intervalo < 0) {
    intervalo = intervalo * -1;
  }
  return intervalo;
}

int generarIntervaloIncorrecto(int intervalo) {
  int intervalo_incorrecto;
  do {
    intervalo_incorrecto = random(1, 10);
  } while (intervalo_incorrecto == intervalo);
  return intervalo_incorrecto;
}

void imprimirOpciones(int opcion_correcta, int opcion_1, int opcion_2, int opcion_3) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(1, 2);

  oled.print("Opcion 1: ");
  oled.println(opcion_1);
  oled.print("Opcion 2: ");
  oled.println(opcion_2);
  oled.print("Opcion 3: ");
  oled.println(opcion_3);
  oled.print("Opcion 4: ");
  oled.print(opcion_correcta);
  oled.display();
}

int leerBotones() {
  while (true) {
    if (digitalRead(BOTON_1) == HIGH) return 1;
    if (digitalRead(BOTON_2) == HIGH) return 2;
    if (digitalRead(BOTON_3) == HIGH) return 3;
    if (digitalRead(BOTON_4) == HIGH) return 4;
  }
}

void verificarRespuesta(int respuesta, int correcta) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(2);
  if (respuesta == correcta) {
    oled.println("Correcto!");
    analogWrite(LED_G, 255); // Enciende LED verde
    delay(1000);
    analogWrite(LED_G, 0); // Apaga LED verde
  } else {
    oled.println("Incorrecto!");
    analogWrite(LED_R, 255); // Enciende LED rojo
    delay(1000);
    analogWrite(LED_R, 0); // Apaga LED rojo
  }
  oled.display();
  delay(2000);
}
