
#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define RxD2 12                                                                          // Usado para conexion rs232 virtual.
#define TxD2 13
#define REPORTING_PERIOD_MS     1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
SoftwareSerial nextion(TxD2, RxD2);
Nextion myNextion(nextion, 57600);
uint32_t tsLastReport = 0;
double SPO2 = 0;
double BPM = 0;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  digitalWrite(4, HIGH);
  Serial.println("Beat!");
  delay(80);
  digitalWrite(4, LOW);
}

void setup()
{
  Serial.begin(115200);
  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  pinMode(4, OUTPUT);//BUZZER
  digitalWrite(4, HIGH);
  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
  digitalWrite(4, LOW);
  myNextion.sendCommand("page 5");
  delay(100);
  myNextion.setComponentText("g0", "Frecuencia Cardiaca y Oximetria");

}

void loop()
{
  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    BPM = pox.getHeartRate();
    SPO2 = pox.getSpO2();
    Serial.print("Heart rate:");
    Serial.print(BPM);
    Serial.print("bpm / SpO2:");
    Serial.print(SPO2);
    Serial.println("%");
    myNextion.setComponentValue("x0", BPM * 10); //delay(2);
    myNextion.setComponentValue("n0", SPO2); //delay(2);
    int intbpm2 = 120;// + BPM;
    char buf2[30];
    const char *first2 = "add 4,1,";
    char str2[8];
    itoa(intbpm2, str2, 10);
    const char *second2 = str2;
    strcpy(buf2, first2);
    strcat(buf2, second2);
    myNextion.sendCommand(buf2);
    tsLastReport = millis();
    myNextion.sendCommand(buf2);
    myNextion.sendCommand(buf2);
    myNextion.sendCommand(buf2);
    myNextion.sendCommand(buf2);
    myNextion.sendCommand(buf2);
    myNextion.sendCommand(buf2);
  }

  int intbpm = 120+BPM;
  char buf1[30];
  const char *first1 = "add 4,1,";
  char str1[8];
  itoa(intbpm, str1, 10);
  const char *second1 = str1;
  strcpy(buf1, first1);
  strcat(buf1, second1);
  myNextion.sendCommand(buf1);

  int intO2 = SPO2;
  char buf3[30];
  const char *first3 = "add 4,2,";
  char str3[8];
  itoa(intO2, str3, 10);
  const char *second3 = str3;
  strcpy(buf3, first3);
  strcat(buf3, second3);
  myNextion.sendCommand(buf3);
}
