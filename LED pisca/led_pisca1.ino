// C++ code
//
int LED=13;
int time=1000;
void setup()
{
  pinMode(LED, OUTPUT);
}

void loop()
{
  digitalWrite(LED, HIGH);
  delay(time);
  digitalWrite(LED,LOW);
  delay(time);
}