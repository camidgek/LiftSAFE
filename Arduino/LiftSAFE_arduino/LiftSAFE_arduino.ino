#include <SoftwareSerial.h>

const int FLEX_PIN = A2; // Pin connected to voltage divider output

//Flex sensor constants
//const float VCC = 3.7; // (XBee) Measured voltage of Ardunio 5V line
//const float VCC = 5.0;
//const float R_DIV = 10000.0; // Measured resistance of 25k resistors (R2)


//Noise smoothing constants begin here.
const int numReadings = 10;
 
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0; // the running total
bool start = true;          
int average = 0;
int endIndex = 10;
int StartPos = 0;
int MapStart = 0;
//SoftwareSerial XBee(11, 10);
 
void setup()
{
// XBee.begin(9600);
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
 
void loop()
{
 
  for (int i = 0; i < endIndex; i++) {
    // subtract the last reading:
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = analogRead(FLEX_PIN);
    //Serial.println("Readings: " + String(readings[readIndex]));
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;
 
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }
  }
  // Calculate the average:
  average = total / numReadings;
  if(start){
    StartPos = average;
    MapStart = StartPos - 35;
  }
  start = false;
  // Expand range
  int MapValue = map(average, MapStart, StartPos, 0, 100);
 
  //Serial.print("Average");
  //Serial.print(average);
  //Serial.print("Value: ");
  Serial.println(MapValue);
 
 
// Threshhold Static
//  if ((MapValue >= 90)) {
//    Serial.print(",GREEN.");
////    }else if ((average < StartPos - 10) && average >= StartPos - 20){
////      Serial.println("YELLOW");
//  } else {
//        Serial.print(",RED.");
//  }
 
  Serial.flush();
  delay(500);
}
