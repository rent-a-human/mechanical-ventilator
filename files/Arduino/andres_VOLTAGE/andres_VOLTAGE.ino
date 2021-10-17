#include <Nextion.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#define SENSORVOLT A0
#define R_OUT1 13
#define R_OUT2 12
#define RxD 5                                                                          // Usado para conexion rs232 virtual.
#define TxD 6
int analogValue = 0;
const int numReadings = 20;

int readings[numReadings];
int readIndex = 0;
int total = 0;
double average = 0;

SoftwareSerial nextion(TxD, RxD);
Nextion myNextion(nextion, 9600);

void setup() {
  pinMode(SENSORVOLT, INPUT);
  pinMode(R_OUT1, OUTPUT);
  digitalWrite(R_OUT1, HIGH);
  Serial.begin(38400);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  myNextion.sendCommand("page 1");
}

void loop() {
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(SENSORVOLT);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total * 2.5 / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  myNextion.setComponentValue("n2", average);
  char buf1[30];
  const char *first1 = "add 1,1,";
  char str1[8];
  itoa((int)average, str1, 10);
  const char *second1 = str1;
  strcpy(buf1, first1);
  strcat(buf1, second1);
  myNextion.sendCommand(buf1);
  delay(2);        // delay in between reads for stability
}

void LeerDivisor() {
  analogValue = analogRead(SENSORVOLT);
  Serial.print("Sensor: ");
  Serial.println(analogValue);
  myNextion.setComponentValue("n2", analogValue * 2);
  delay(100);
}
