// Primero definimos variables para las entradas físicas del Arduino y sus respectivos componentes.
#define BUZZER 9
#define BOTON_1 2 
#define BOTON_2 4
#define BOTON_3 7
#define BOTON_4 8

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
double intervalos[] = {4, 5, 7, 9, -4, -5, -7, -9};

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600); // Iniciamos la comunicación serial
  
  // Definimos los pines como entradas o salidas
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTON_1, INPUT);
  pinMode(BOTON_2, INPUT);
  pinMode(BOTON_3, INPUT);
  pinMode(BOTON_4, INPUT);
}

void loop() {
  // Por ahora no será muy sofisticado el sistema. Elegiré una nota del medio para que nunca me tire error al elegir la segunda nota.
  // Primero elegimos la primera nota al azar

  // Elegimos la primer nota al azar
  int numero_random = random(9, 17); 
  double primer_nota = notas_musicales[numero_random]; // En este caso, se podrá elegir desde la novena nota hasta la décimosexta.
  Serial.print("Esta es la primer nota: ");
  Serial.println(primer_nota);
  
  // Elegimos al azar el intervalo
  int intervalo = intervalos[random(0,7)];
  Serial.print("Este es el intervalo: ");
  Serial.println(intervalo);

  // Definimos la segunda nota en base al intervalo
  double segunda_nota = notas_musicales[numero_random + intervalo];

  Serial.print("Este es la segunda nota: ");
  Serial.println(segunda_nota);

  Serial.println("-------");

 // Ahora emitimos los sonidos
 tone(BUZZER, primer_nota, 200);
 delay(400);
 tone(BUZZER, segunda_nota, 200);
 delay(400);

 delay(2700); 
}
