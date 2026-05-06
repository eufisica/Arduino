int ledPin = 12;
int sensorPin1 = 2;
int long ranDelay = 0;
int  sensorValue = 0;
float realTime;

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin1, INPUT);
}

void loop(){
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("Push the button to start game");
  
  while (digitalRead(sensorPin1)==  0) {
  }
  digitalWrite(ledPin, HIGH);
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("Get Ready!");
  delay(1000);
  Serial.println("Get Set!");
  delay(1000);
  ranDelay = random(5000);
  delay(ranDelay);
  digitalWrite(ledPin, LOW);
  Serial.println("Go!");
  realTime = millis();
  
 
  
  while  (digitalRead(sensorPin1)== 0) {
    
  }
  digitalWrite(ledPin, HIGH);
  Serial.println("Your time was");
  realTime = millis()-realTime;
  Serial.print(realTime/1000,2);
  Serial.println(" seconds");
 delay(2000); 
}
