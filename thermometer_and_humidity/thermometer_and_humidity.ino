// C++ code
// define contants
const int sensorPin1 = A0; //temperature sensor -> analogic door
const int sensorPin2 = A1; //humidity sensor -> analogic door (read between 0 and 1024 bits)
const float baselineTemp = 15.0; //value of the base temperature
const int dry = 839; //value for dry sensor -> humidity sensor
const int wet = 470; //value for wet sensor -> humidity sensor

void setup()
{
  pinMode(2, OUTPUT); //LED on digital port 2 (2 values, 1 or 0)
  pinMode(3, OUTPUT); //LED on digital port 3
  pinMode(4, OUTPUT); //LED on digital port 4

  Serial.begin(9600); //open serial port
  for(int pinNumber = 2; pinNumber<5; pinNumber++)
  {
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);
  }
}

void loop()
{
  int sensorVal1 = analogRead(sensorPin1);
  Serial.print("Sensor Value: ");
  Serial.print(sensorVal1);
  Serial.println();
  //convert the lecture into tension
  float voltage = (sensorVal1/1024.0)*5.0;
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print(" V");
  Serial.println();
  //convert voltage into temperature
  float temperature = (voltage - .5)*100;
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" graus C");
  Serial.println();

  if(temperature < baselineTemp){
    digitalWrite(2,LOW);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
  } else if(temperature >= baselineTemp+2 && temperature < baselineTemp+4){
    digitalWrite(2,HIGH);
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
  } else if(temperature >= baselineTemp+4 && temperature < baselineTemp+6){
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(4,LOW);
  } else if(temperature >= baselineTemp+6){
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(4,HIGH);
  }

  int sensorVal2 = analogRead(A1); //to store the value read in the sensor
  
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A1));

  // the sensor has a range of 470 to 839
  // we want to translate to a scale of 0% (dry) to 100% (wet)
  int percentageHumidity = map(sensorVal2, wet, dry, 100, 0);
  
  Serial.print(percentageHumidity); //to avoid a new line
  Serial.println("%"); //to start a new line
  delay(5000); //wait 5 seconds to start this cycle again   
}