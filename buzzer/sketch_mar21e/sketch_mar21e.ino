int piezoBuzzer = 10;
void setup() {
  // put your setup code here, to run once:
pinMode(piezoBuzzer,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    
    digitalWrite (piezoBuzzer, HIGH); // switch on buzzer
    tone(piezoBuzzer, 1200, 1000); //  1 s duration
      delay (1000);           // wait 1 second
    digitalWrite (piezoBuzzer, LOW);  // switch off buzzer
    tone(piezoBuzzer, 700, 1000); //  1 s duration
      delay (1000);
}
