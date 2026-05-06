float distance;
float width;
float speed;
int signal=A0;
void setup()
{
  Serial.begin(9600);
  
}

void loop()
{
  pinMode(signal, OUTPUT);
  digitalWrite(signal,LOW);
  delayMicroseconds(100);
  digitalWrite(signal,HIGH);
  delayMicroseconds(10);
  digitalWrite(signal,LOW);
  
  pinMode(signal, INPUT);
  
  width = pulseIn(signal, HIGH);
               //calculating speed of sound
  //speed = 100.1*10*3600*2/width;
 
               //calculating distance of a object
  width=width/1000000;
  distance=34300*width/2;
  Serial.print(distance);
  Serial.println("m");
  delay(500);
  
}