#define BUZZER 9
#define BOTON_1 2 
#define BOTON_2 4
#define BOTON_3 7
#define BOTON_4 8

void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTON_1, INPUT);
  pinMode(BOTON_2, INPUT);
  pinMode(BOTON_3, INPUT);
  pinMode(BOTON_4, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Leer el estado de los botones
int  valor_1 = digitalRead(BOTON_1); // Lectura del Botón 1
int  valor_2 = digitalRead(BOTON_2); // Lectura del Botón 2
int  valor_3 = digitalRead(BOTON_3); // Lectura del Botón 3
int  valor_4 = digitalRead(BOTON_4); // Lectura del Botón 4

  // Evaluar si los valores son altos (HIGH) o bajos (LOW)
  if (valor_1 == HIGH) {
    tone(BUZZER, 440);
  } else if (valor_2 == HIGH) {
    tone(BUZZER, 523.25);
  } else if (valor_3 == HIGH) {
    tone(BUZZER, 659.25);
  } else if (valor_4 == HIGH) {
    tone(BUZZER, 880);
  } else {
    noTone(BUZZER);
  }
}
