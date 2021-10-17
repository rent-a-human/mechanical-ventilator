#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Nextion.h>
#define RxD 10                                                                          // Usado para conexion rs232 virtual.
#define TxD 11
#define MOTOR1 4
#define MOTOR2 5
#define MOTOR3 6
#define MOTOR4 7
#define SENSOR A1
#define ENLACE A2

int pausa_set = 0;
int rango_set = 0;
int delay_set = 0;
int p_asc = 7;
int t_pausa = 500;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
int senal = 0;
// variables to hold the parsed data
char message[numChars] = {0};
int data1 = 0;
int data2 = 0;
int data3 = 0;
int data4 = 0;
int data5 = 0;
int encendido = 0;
int gate = 0;
int sensor = 0;
int delay1 = 500;
int delay2 = 500;
int contador = 0;

boolean newData = false;
float pausa = 0;
int pausar = 30;
float pausa1 = 15;
float pausa2 = 40;
int PAMOTOR = 0;
int cont = 0;
int frec = 0;
int rango = 90; //rango de 0 a 100, equv de 50 a 700
long ciclos = 0;
SoftwareSerial portOne(10, 11);

void setup()
{

  Serial.begin(38400);
  portOne.begin(38400);
  pinMode(SENSOR, INPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  pinMode(ENLACE, OUTPUT);
  analogWrite(ENLACE, 111);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  Serial.println("arrancando");
  Reinit();
  //while (true) {
  //Test();
  //}

  Serial.println("En linea");
}

void UnCiclo() {
  analogWrite(ENLACE, 255);
  Reiniciar();
  pausa = pausa_set;
  //int rangotest = 100;
  RetroMotorACC2(62 * rango_set);
  //RetroMotorACC2(62 * rango_set);
  delay(t_pausa);
  pausa = p_asc;//7
  AvanceMotorACC(62 * rango_set);
  //delay(50);
  delay(delay_set);

  analogWrite(ENLACE, 0);
}

void loop ()
{

  //gate = analogRead(A2);
  //if (encendido == 1 or gate > 100) {//GATE HASTA 1023
  if (encendido == 1) {
    digitalWrite(8, LOW);
    Serial.println("Arrancando");
    if (ciclos < 1) {
      Reiniciar();
    }
    UnCiclo();
    ciclos++;
    encendido = 0;
    digitalWrite(8, HIGH);
  } else {
    //ciclos = 0;
    recvWithStartEndMarkers();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      // this temporary copy is necessary to protect the original data
      //   because strtok() used in parseData() replaces the commas with \0
      parseData();
      showParsedData();
      newData = false;
    }
  }
  //Serial.println("en linea");
}

void AvanceMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(MOTOR1,  LOW);
    digitalWrite(MOTOR2,  HIGH);
    digitalWrite(MOTOR3,  LOW);
    digitalWrite(MOTOR4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  HIGH);
    digitalWrite(MOTOR2,  LOW);
    digitalWrite(MOTOR3,  LOW);
    digitalWrite(MOTOR4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  HIGH);
    digitalWrite(MOTOR2,  LOW);
    digitalWrite(MOTOR3,  HIGH);
    digitalWrite(MOTOR4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  LOW);
    digitalWrite(MOTOR2,  HIGH);
    digitalWrite(MOTOR3,  HIGH);
    digitalWrite(MOTOR4,  LOW);
    delayMicroseconds(pausa);
    //analogWrite(ENLACE, pausa);
    //PAMOTOR = PAMOTOR - 1;
    //senal = abs(PAMOTOR) / 100;


  }
  /*/senal = map(PAMOTOR, 0, 60000, 0, 255);
    PAMOTOR = PAMOTOR + steps;
    senal = 0;
    analogWrite(ENLACE, senal);*/

}

void RetroMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(MOTOR1,  LOW);
    digitalWrite(MOTOR2,  HIGH);
    digitalWrite(MOTOR3,  LOW);;
    digitalWrite(MOTOR4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  LOW);
    digitalWrite(MOTOR2,  HIGH);
    digitalWrite(MOTOR3,  HIGH);
    digitalWrite(MOTOR4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  HIGH);
    digitalWrite(MOTOR2,  LOW);
    digitalWrite(MOTOR3,  HIGH);
    digitalWrite(MOTOR4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(MOTOR1,  HIGH);
    digitalWrite(MOTOR2,  LOW);
    digitalWrite(MOTOR3,  LOW);
    digitalWrite(MOTOR4,  HIGH);
    delayMicroseconds(pausa);

  }
  /*PAMOTOR = PAMOTOR - steps;
    senal = 255;
    analogWrite(ENLACE, senal);
    //senal = map(PAMOTOR, 0, 30000, 0, 255);
    //analogWrite(ENLACE, pausa);
    //analogWrite(ENLACE, senal);
    //Serial.println(PAMOTOR);
  */
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';//<pausa1, pausa2, rango2 / 5, delay1, delay2>
  char endMarker = '>';
  char rc;

  while (portOne.available() > 0 && newData == false) {
    rc = portOne.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  encendido = atoi(strtokIndx);     // convert this part to an integer

  //strcpy(message, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  data1 = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  data2 = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  data3 = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  data4 = atoi(strtokIndx);     // convert this part to an integer

}

//============

void showParsedData() {
  pausa_set = data1;
  rango_set = data2;
  delay_set = data3;
  t_pausa = data4;
  Serial.print("En Movimiento ");
  Serial.println(encendido);
}

void Reiniciar() {
  //myNextion.setComponentText("g0", "Reiniciando");
  if (ciclos < 1) {
    Reinit();
    return;
  }
  pausa = 30;
  sensor = analogRead(SENSOR);
  ////Serial.print("Sensor Carro: ");
  ////Serial.println(sensor_carro);
  while (sensor > 300) {
    contador++;
    sensor = analogRead(SENSOR);
    AvanceMotor(1);
    if (contador > 6000) {
      RetroMotor(3000);
      contador = 0;
    }

  }
  //delay(50);
  while (sensor <= 100) {
    sensor = analogRead(SENSOR);
    RetroMotor(1);
  }
  PAMOTOR =  0;
  contador = 0;
}


void Reinit() {
  Serial.println("Reiniciando");
  pausa = 30;
  contador = 0;
  RetroMotor(500);
  delay(200);
  sensor = analogRead(SENSOR);
  ////Serial.print("Sensor Carro: ");
  ////Serial.println(sensor_carro);
  while (sensor > 300) {
    contador++;
    sensor = analogRead(SENSOR);
    AvanceMotor(1);
    if (contador > 6000) {
      RetroMotor(3000);
      contador = 0;
    }

  }
  delay(200);
  sensor = analogRead(SENSOR);
  while (sensor <= 100) {
    sensor = analogRead(SENSOR);
    RetroMotor(1);
  }
  PAMOTOR =  0;
  contador = 0;
}

void AvanceMotorACC2(float steps) {
  int pinit = pausa;
  float p0, p1, p2, p3, p4, pa;
  p4 = 1 * pausa;
  p3 = 1 * pausa;
  p2 = 1.5 * pausa;
  p1 = 2 * pausa;
  p0 = 2.5 * pausa;
  pa = 1 * pausa;
  pausa = p0 ;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (4 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p1;

  for (int i = 0; i < (4 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p2;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (4 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p3;

  for (int i = 0; i < (2 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p4;
  Serial.println(PAMOTOR);
  for (long i = 0; i < ((steps * 86) / 100); i++) {
    AvanceMotor(1);
  }
  Serial.println(PAMOTOR);
  //pausa = pinit;
}

//////////////


void RetroMotorACC(float steps) {
  int pinit = pausa;
  float p0, p1, p2, p3, p4;
  p4 = 1 * pausa;
  p3 = 2 * pausa;
  p2 = 3 * pausa;
  p1 = 4 * pausa;
  p0 = 6 * pausa;
  pausa = p0 ;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (4 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p1;

  for (int i = 0; i < (2 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p2;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (2 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p3;

  for (int i = 0; i < (2 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p4;
  Serial.println(PAMOTOR);
  for (long i = 0; i < ((steps * 86) / 100); i++) {
    RetroMotor(1);
  }

  pausa = p3;

  for (int i = 0; i < (2 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p2;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (2 * steps / 100); i++) {
    RetroMotor(1);
  }
  Serial.println(PAMOTOR);
  pausa = pinit;
}

void AvanceMotorACC(float steps) {
  int pinit = pausa;
  float p0, p1, p2, p3, p4, pa, pb;
  p4 = 1 * pausa;
  p3 = 2 * pausa;
  p2 = 3 * pausa;
  pb = 4 * pausa;
  p1 = 5 * pausa;
  pa = 6 * pausa;
  p0 = 7 * pausa;
  pausa = p0 ;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (2 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = pa ;
  for (int i = 0; i < (3 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p1;
  for (int i = 0; i < (3 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = pb ;
  for (int i = 0; i < (3 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p2;
  for (int i = 0; i < (3 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p3;
  for (int i = 0; i < (2 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p4;
  Serial.println(PAMOTOR);
  for (long i = 0; i < ((steps * 80) / 100); i++) {
    AvanceMotor(1);
  }

  pausa = p3;

  for (int i = 0; i < (2 * steps / 100); i++) {
    AvanceMotor(1);
  }
  pausa = p2;
  Serial.println(PAMOTOR);
  for (int i = 0; i < (2 * steps / 100); i++) {
    AvanceMotor(1);
  }
  Serial.println(PAMOTOR);
  //pausa = pinit;
}

void RetroMotorACC2(float steps) {
  int pinit = pausa;
  float p0, p1, p2, p3, p4, pa;
  p4 = 1 * pausa;
  p3 = 2 * pausa;
  p2 = 3 * pausa;
  p1 = 4 * pausa;
  p0 = 5 * pausa;
  pausa = p0 ;
  Serial.println(PAMOTOR);
  for (int i = 0; i < steps / 100; i++) {
    RetroMotor(1);
  }
  pausa = p1;

  for (int i = 0; i < steps / 100; i++) {
    RetroMotor(1);
  }
  pausa = p2;

  for (int i = 0; i < (steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p3;

  for (int i = 0; i < (steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p4;
  Serial.println(PAMOTOR);
  for (long i = 0; i < ((steps * 70) / 100); i++) {
    RetroMotor(1);
  }

  pausa = p3;
  for (int i = 0; i < (8 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p2;

  for (int i = 0; i < (7 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p1;

  for (int i = 0; i < (6 * steps / 100); i++) {
    RetroMotor(1);
  }
  pausa = p0;
  for (int i = 0; i < (5 * steps / 100); i++) {
    RetroMotor(1);
  }
  Serial.println(PAMOTOR);
  pausa = pinit;
}
