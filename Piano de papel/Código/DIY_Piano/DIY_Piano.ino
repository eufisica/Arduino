/* DIY Piano

   - DIY Piano with Arduino. This is a very simple project.
   - ONLY needed some very cheap materials,
     e.g. pencil, A4 paper, paper clip, speaker, male-to-male jumper, breadboard and Arduino Uno board.
   - Of course the Arduino Uno board is not cheap, but it is reusable for other projects too. So it is worth to own one.
   
   - This project using the concept of capasitive sensor, so no switch/button is required for it.
   - Here is the link of capacitive sensor. For better understanding, please visit the page.
     http://playground.arduino.cc//Main/CapacitiveSensor?from=Main.CapSense
     
   - To watch the Demo of This Project:
     http://www.youtube.com/watch?v=X4QNT5hOHLs&list=TLlm-tAV1gEzDF4VV39VdjCSLNlfe0tTpU


   Created by:
   Oh Hai Seng || Junny Oh    07 November 2013
*/



// Import the CapacitiveSensor Library.
#include <CapacitiveSensor.h>


// Name the pin as led. 
#define speaker 11


// Set the Send Pin & Receive Pin.
CapacitiveSensor   cs_2_3 = CapacitiveSensor(2,3);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_2_4 = CapacitiveSensor(2,4);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_2_5 = CapacitiveSensor(2,5);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_2_6 = CapacitiveSensor(2,6);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_2_7 = CapacitiveSensor(2,7);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_2_8 = CapacitiveSensor(2,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_2_9 = CapacitiveSensor(2,9);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil


void setup()                    
{
  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  
  // Arduino start communicate with computer.
  Serial.begin(9600);
}

void loop()                    
{
  // Set a timer.
  long start = millis();
  
  // Set the sensitivity of the sensors.
  long total1 =  cs_2_3.capacitiveSensor(30);
  long total2 =  cs_2_4.capacitiveSensor(30);
  long total3 =  cs_2_5.capacitiveSensor(30);
  long total4 =  cs_2_6.capacitiveSensor(30);
  long total5 =  cs_2_7.capacitiveSensor(30);
  long total6 =  cs_2_8.capacitiveSensor(30);
  long total7 =  cs_2_9.capacitiveSensor(30);
  


  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing

  Serial.print(total1);                  // print sensor output 1
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.print(total2);                  // print sensor output 2
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.print(total3);                  // print sensor output 3
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.print(total4);                  // print sensor output 4
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.print(total5);                  // print sensor output 5
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.print(total6);                  // print sensor output 6
  Serial.print("\t");                    // Leave some space before print the next output
  Serial.println(total7);                // print sensor output 7
                                         // "println" - "ln" represent as "line", system will jump to next line after print the output.
  
  
  
  
  // When hand is touched the sensor, the speaker will produce a tone.
  // I set a threshold for it, so that the sensor won't be too sensitive.
  if (total1 > 150) tone(speaker,523);
  if (total2 > 150) tone(speaker,587);
  if (total3 > 150) tone(speaker,659);
  if (total4 > 150) tone(speaker,698);
  if (total5 > 150) tone(speaker,784);
  if (total6 > 150) tone(speaker,880);
  if (total7 > 150) tone(speaker,988);
  
  // When hand didn't touch on it, no tone is produced.
  if (total1<=150  &  total2<=150  &  total3<=150 & total4<=150  &  total5<=150  &  total6<=150 &  total7<=150)
    noTone(speaker);

  delay(10);                             // arbitrary delay to limit data to serial port 
}
