// # Editor     : roker, eufisica
// # Date       : 17.12.2025

// # Sensor: URM09 Ultrasonic Sensor(Gravity Analog)(V1.0)
// # Sensor SKU : SEN0307
// # Version     : 1.0


#define  MAX_RANG      (520)//the max measurement vaule of the module is 520cm(a little bit longer than  effective max range)
#define  ADC_SOLUTION      (1023.0)//ADC accuracy of Arduino UNO is 10bit

int sensityPin = A0;    // select the input pin 
//int dist = 15; // distance to the sensor
void setup() {
  // Serial init
  Serial.begin(9600);
}
float dist_t, sensity_t, v_sound;
void loop() {
  // read the value from the sensor:
 sensity_t = analogRead(sensityPin);
  // turn the ledPin on

 dist_t = sensity_t * MAX_RANG  / ADC_SOLUTION;//
 Serial.print(dist_t,0);
 Serial.println("cm");
 
 v_sound = (dist_t/sensity_t)*(100000/100);
 Serial.print(v_sound,0);
 Serial.println("m/s");

 delay(1000);

}