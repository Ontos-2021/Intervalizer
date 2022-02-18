// Primero definimos variables para las entradas físicas del Arduino y sus respectivos componentes.
#define BUZZER 9
#define BOTON_1 2 
#define BOTON_2 4
#define BOTON_3 7
#define BOTON_4 8
#define LED_R 6
#define LED_G 5
#define LED_B 3

// Declaramos varialbes para la pantalla OLED

// Estas son las librerías
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Estas son las dimensiones de la pantalla OLED
#define ANCHO 128
#define ALTO 64

// Acá se instancia el objeto "oled" de la clase "Adafruit_SSD1306".
// La variable OLED_RESET es propia de un módulo de pantalla fabricado por Adafruit.
#define OLED_RESET 4 
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);

// Array con las frecuencias de las notas músicales. Son dos octavas, de La4 a La6
double notas_musicales[] = {
  440,
  466.16,
  493.88,
  523.25,
  554.37,
  587.33,
  622.25,
  659.26,
  698.46,
  739.99,
  783.99,
  830.61,
  880,
  932.33,
  987.77,
  1046.5,
  1108.73,
  1174.66,
  1244.51,
  1318.51,
  1396.91,
  1479.98,
  1567.98,
  1661.22,
  1760
};

// Definimos los intervalos disponibles. En este caso, sólo serán 3ra Mayor, 4ta Justa, 5ta Justa y 6ta mayor.
int intervalos[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -2, -3, -4, -5, -6, -7, -8, -9};

// Variables globales
int intervalo_incorrecto_1, intervalo_incorrecto_2, intervalo_incorrecto_3;
int opcion_1, opcion_2, opcion_3, opcion_4;

void setup() {
  // put your setup code here, to run once:
  
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

  // RandomSeed - Esto permitirá que la secuencia de números
  // siempre sea distinta.
  randomSeed(analogRead(A0));

  // Iniciamos la pantalla OLED
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0X3C);

  // Bienvenida al juego
  saludar();
}

void loop() {
  // Primero elegimos la primera nota al azar

  // Elegimos la primer nota al azar
  int numero_random = random(9, 17); 
  double primer_nota =elegirPrimerNota(numero_random);
  
  // Elegimos al azar el intervalo
  int intervalo = elegirIntervalo();

  // Definimos la segunda nota en base al intervalo
  double segunda_nota = elegirSegundaNota(intervalo, numero_random);

  Serial.println("-------");

 // Ahora generamos las opciones posibles
 // En este caso serán opcion_correcta, opcion_1, opcion_2, opcion_3
 // Las 3 restantes serán incorrectas y distintas entre sí.
 
 int intervalo_correcto = definirIntervaloCorrecto(intervalo);
 generarIntervalosIncorrectos(intervalo);
 
 // Aquí habría que llamar a la función "revolver"
 revolverOpciones();
  
 // Ahora imprimimos en la pantalla las opciones
 imprimirOpciones(intervalo_correcto, intervalo_incorrecto_1, intervalo_incorrecto_2, intervalo_incorrecto_3);

 // Ahora emitimos los sonidos
 emitirIntervalos(primer_nota, segunda_nota);
}

void saludar(){
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
  // Por ahora no será muy sofisticado el sistema. Elegiré una nota del medio para que nunca me tire error al elegir la segunda nota.
  double primer_nota = notas_musicales[numero_random]; // En este caso, se podrá elegir desde la novena nota hasta la décimosexta.
  Serial.print("Esta es la primer nota: ");
  Serial.println(primer_nota);
  return primer_nota;
}

int elegirIntervalo() {
  int intervalo = intervalos[random(0,17)];
  Serial.print("Este es el intervalo: ");
  Serial.println(intervalo);
  return intervalo;
}

double elegirSegundaNota(int intervalo, int numero_random) {
  double segunda_nota = notas_musicales[numero_random + intervalo];
  Serial.print("Este es la segunda nota: ");
  Serial.println(segunda_nota);
  return segunda_nota;
}

void emitirIntervalos(double primer_nota, double segunda_nota){
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

// Funciones relacionadas con definir las opciones

int definirIntervaloCorrecto(int intervalo){
  if (intervalo < 0) {
    intervalo = intervalo * -1;
  }
  return intervalo;
}

void generarIntervalosIncorrectos(int intervalo) {
  intervalo_incorrecto_1 = random(0, 11);
  while (intervalo == intervalo_incorrecto_1) {
    intervalo_incorrecto_1 = random(0, 11);
  };

  intervalo_incorrecto_2 = random(0, 11);
  while (intervalo_incorrecto_1 == intervalo_incorrecto_2 || intervalo_incorrecto_2 == intervalo) {
    intervalo_incorrecto_2 = random(0, 11);
  }
  intervalo_incorrecto_3 = random(0, 11);
  while (intervalo_incorrecto_2 == intervalo_incorrecto_3 || intervalo_incorrecto_1 == intervalo_incorrecto_3 || intervalo_incorrecto_3 == intervalo) {
    intervalo_incorrecto_3 = random(0, 11);
  }
}

void revolverOpciones() {
  //Serial.println("Se activo la función Revolver opciones"); // Al parecer esta línea buggea
  Serial.println("Hola!"); // Y por alguna extraña razón esta línea no buggea
  boolean flag = false;
  // Primero crearé un algoritmo para revolver los index. En este caso de 0 a 3.
  int indexes[4];
  indexes[0] = random(0, 3);
  while (!flag) {
    indexes[1] = random(0, 3);
    if (indexes[0] != indexes[1]) {
      flag = true;
    }
  };
  
  flag = false;
  while (!flag) {
    indexes[2] = random(0, 3);
    if (indexes[0] != indexes[2] || indexes[1] != indexes[2]) {
      flag = true;
    }
  };

  flag = false;
  while (!flag) {
    indexes[3] = random(0, 3);
    if (indexes[0] != indexes[3] || indexes[1] != indexes[3] || indexes[2] != indexes[3]) {
      flag = true;
    }
  };

  

  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(1, 2);
  //for (byte i = 0; i < 4; i = i + 1) {
  //  oled.println(indexes[i]);
  //};

  oled.print("Test");
  
  oled.display();
  delay(3000);
  int x;
  while (x < 4){
    Serial.println(indexes[x]);
    x++;
  }

  //for (byte i = 0; i < 5; i = i + 1) {
  //Serial.println(indexes[i]);
  //}
}

void imprimirOpciones(int opcion_correcta, int opcion_1, int opcion_2, int opcion_3) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(1, 2);
  
  oled.print("Opcion 1: ");
  oled.print(opcion_1);
  oled.println(" st");
  
  oled.print("Opcion 2: ");
  oled.print(opcion_2);
  oled.println(" st");
  
  oled.print("Opcion 3: ");
  oled.print(opcion_3);
  oled.println(" st");

  // Por alguna razón cuando coloco "Opcion 4: ", se buggea. Y cuando es "Opcion 3: ", no.
  oled.print("Opcion 3: ");
  oled.print(opcion_correcta);
  oled.print(" st");
  
  oled.display();
}
