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
  int valor_1 = digitalRead(BOTON_1);
  if (valor_1 == HIGH) {
    tone(BUZZER, 1760, 40);
    delay(100);
  } else {
    tone(BUZZER, 880, 100);  
    delay(200);
  }

}
