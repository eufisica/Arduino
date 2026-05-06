#include <Servo.h>

int trig = A5;
int echo = A4;
int ledopen = 9;
int ledclose = 11;
int contagem = 0;
int contagemFecho = 0;

Servo myservo;

int pos = 0;
bool isOpen = false;

void setup() {
  myservo.attach(3);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(ledopen, OUTPUT);
  pinMode(ledclose, OUTPUT);
  Serial.begin(9600);
  myservo.write(110); // start closed
  digitalWrite(ledopen, LOW);
  digitalWrite(ledclose, HIGH);
}

void loop() {
  long duration;
  float cm;
  // Trigger the ultrasonic sensor
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Read echo time and convert to cm
  duration = pulseIn(echo, HIGH);
  cm = duration * 0.034 / 2;

  // print out the distance between ultrasonic sensor and object
  Serial.print("Distância: ");
  Serial.print(cm);
  Serial.println("cm");
  // Open gate if object is detected

  if (cm < 15) {
    contagem++;
  } else {
    contagem = 0;
  }

  if (contagem >= 4 && !isOpen) {
    for (pos = 110; pos >= 60; pos--) {
      myservo.write(pos);
      delay(30);
    }
    digitalWrite(ledclose, LOW);
    digitalWrite(ledopen, HIGH);
    Serial.print("PONTE ABERTA\nPONTE ABERTA\nPONTE ABERTA\n");
    isOpen = true;
}

  if (cm >= 15) {
    contagemFecho++;
  } else {
    contagemFecho = 0;
  }

// Close gate if object is no longer detected
if (contagemFecho >= 4 && isOpen) {

  delay(5000);
  digitalWrite(ledopen, LOW);
  digitalWrite(ledclose, HIGH);
  delay(1000);
  for (pos = 60; pos <= 110; pos++) {
  myservo.write(pos);
  delay(30);
  }
  Serial.print("PONTE FECHADA\nPONTE FECHADA\nPONTE FECHADA\n");
  isOpen = false;
}

delay(200); // small delay between readings

}