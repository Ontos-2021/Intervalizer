#define BUZZER 9

void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  tone(BUZZER, 880, 5000);
  // noTone(BUZZER);
  delay(4500);
  tone(BUZZER, 880*2, 5000);
  // noTone(BUZZER);
  delay(4500);
}
