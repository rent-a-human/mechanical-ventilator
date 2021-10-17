#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Nextion.h>

#define RxD 12                                                                          // Usado para conexion rs232 virtual.
#define TxD 13

#define MOTOR1 4
#define MOTOR2 5
#define MOTOR3 6
#define MOTOR4 7

#define M1 8
#define M2 9
#define M3 10
#define M4 11

#define SENSOR1 A0
#define PEDAL A6

#define POT1 A3
#define POT2 A4
#define BUZZER A5

#define RELAY1 2
#define RELAY2 3

int cambio = 0;
int pagina = 0;
int pedal = 0;
int pausa = 15;
int pot1 = 0;
int pot2 = 0;
int sensor1 = 0;
long avance = 0;
int modo = 1; //Secuencial = 1, Paralelo = 2
int modof = 0; //0: Giro normal, 1: reversa
int s1 = 0;
int s2 = 0;
int tiempo1 = 0;
int tiempo2 = 0;
int pasos_por_vuelta = 1600;
float pi = 3.14159;
//float perimetro = 0;
double longitud = 0;
double l_m = 0;
long ciclos = 0;
long contador = 0;
int diametro = 50;
float perimetro = 157;

int diametroAUX = 0;
int avanceAUX = 0;
int tiempo1AUX = 0;
int tiempo2AUX = 0;
int automatico = 0;

SoftwareSerial nextion(TxD, RxD);
Nextion myNextion(nextion, 9600);
//Nextion myNextion(nextion, 115200);

void writeLongIntoEEPROM(int address, long number)
{
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}
long readLongFromEEPROM(int address)
{
  return ((long)EEPROM.read(address) << 24) +
         ((long)EEPROM.read(address + 1) << 16) +
         ((long)EEPROM.read(address + 2) << 8) +
         (long)EEPROM.read(address + 3);
}

void setup() {

  /*for (int i = 10; i < 30; i++) {
    EEPROM.write(i, 0);
    delay(20);
    }*/

  pinMode(SENSOR1, INPUT);
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, HIGH);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  Serial.begin(9600);
  //EEPROM.write(2, 20);
  //EEPROM.write(3, 30);
  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  avance = EEPROM.read(0);
  modo = EEPROM.read(1);
  tiempo1 = EEPROM.read(2);
  tiempo2 = EEPROM.read(3);
  diametro = EEPROM.read(4);
  //perimetro = pi * diametro;//(mm)
  ciclos = readLongFromEEPROM(10);
  contador = readLongFromEEPROM(20);
  longitud = pi * diametro * ciclos;
  l_m = longitud / 1000;
  delay(300);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(BUZZER, LOW);/*
  while (true) {
    myNextion.sendCommand("bauds=9600");
    delay(10);
  }*/
  myNextion.sendCommand("page 1");

}

void loop() {
  //LeerSensores();
  LeerPantalla();
  //LeerPot();
}

void LeerPot() {
  pot1 = analogRead(POT1);
  pausa = map(pot1, 1023, 0, 100, 12);
  myNextion.setComponentValue("x0", pausa);
}

void LeerPantalla() {
  pagina = myNextion.getComponentValue("pagina");
  //delay(10);
  //Serial.println(pagina);

  if (pagina == 1) {
    int bt1p1 = myNextion.getComponentValue("bt1");//FUNCIONANDO
    if (bt1p1 == 1) {
      sensor1 = analogRead(SENSOR1);
      pedal = analogRead(PEDAL);
      //perimetro = pi * diametro;
      if (pedal < 200 && sensor1 < 200) {
        pedal = analogRead(PEDAL);
        sensor1 = analogRead(SENSOR1);
        Main();
        ciclos++;

        if (3 == 0) {
          digitalWrite(BUZZER, HIGH);
          writeLongIntoEEPROM(10, ciclos);
          writeLongIntoEEPROM(20, contador);
          longitud = pi * diametro * ciclos;
          long esc1 = longitud / 100;
          long esc2 = contador / 100;
          digitalWrite(BUZZER, LOW);
          myNextion.setComponentValue("n1", ciclos); //delay(2);
          //AvanceMotor(100);
          myNextion.setComponentValue("n2", esc2); //delay(2);
          //AvanceMotor(100);
          myNextion.setComponentValue("n3", esc1); //delay(2);
          //AvanceMotor(100);
          myNextion.setComponentValue("n4", esc2); //delay(2);
          //AvanceMotor(100);

        }

      } else if (pedal > 200) {
        digitalWrite(BUZZER, HIGH);
        myNextion.setComponentValue("bt1", 0);
        delay(1000);
        digitalWrite(BUZZER, LOW);
      }
    } else {
      //delay(50);
      modof = myNextion.getComponentValue("bt3");
      cambio = myNextion.getComponentValue("cambio");
      while (cambio < 0) {
        cambio = myNextion.getComponentValue("cambio");
        //delay(10);
      }
      if (cambio == 0) {
        myNextion.setComponentValue("n0", diametro); delay(2);
      } else if (cambio == 1) {
        diametroAUX = myNextion.getComponentValue("n0");  delay(10);
        if (diametroAUX > 1) {
          diametro = diametroAUX;
          EEPROM.write(4, diametro);
        }

        int bt0p1 = myNextion.getComponentValue("bt0");
        if (bt0p1 == 1) {
          ciclos = 0;
          for (int i = 10; i < 20; i++) {
            EEPROM.write(i, 0);
          }
          myNextion.setComponentValue("bt0", 0);
        }
        int bt2p1 = myNextion.getComponentValue("bt2");
        if (bt2p1 == 1) {
          contador = 0;
          for (int i = 20; i < 30; i++) {
            EEPROM.write(i, 0);
          }
          myNextion.setComponentValue("bt2", 0);
        }
        longitud = pi * diametro * ciclos;
        myNextion.setComponentValue("cambio", 0);
      }
    }
    longitud = pi * diametro * ciclos;
    long esc1 = longitud / 100;
    long esc2 = contador / 100;
    myNextion.setComponentValue("n1", ciclos); delay(2);
    myNextion.setComponentValue("n2", esc2); delay(2);
    myNextion.setComponentValue("n3", esc1); delay(2);
    myNextion.setComponentValue("n4", esc2); delay(2);
  }
}

void Main() {
  //digitalWrite(BUZZER, HIGH);
  diametroAUX = myNextion.getComponentValue("n0");  delay(10);
  if (diametroAUX > 1) {
    diametro = diametroAUX;
  }
  pot1 = analogRead(POT1);
  avance = pi * diametro;
  //map(value, fromLow, fromHigh, toLow, toHigh)
  int steps = map(avance, perimetro, 0, pasos_por_vuelta, 0);
  pausa = map(pot1, 1023, 0, 100, 15);
  //digitalWrite(BUZZER, LOW);
  if (modof == 0) {
    AvanceMotor(steps);
  } else {
    RetroMotor(steps);
  }

  contador = contador + avance;
  // digitalWrite(BUZZER, HIGH);
  //delay(50);

}

void LeerSensores() {
  sensor1 = analogRead(SENSOR1);
  pot1 = analogRead(POT1);
  pot2 = analogRead(POT2);
  pedal = analogRead(PEDAL);
  Serial.print("Sensor 1: ");
  Serial.print(sensor1);
  Serial.print(" Pot 1: ");
  Serial.print(pot1);
  Serial.print(" Pot 2: ");
  Serial.println(pot2);
  //myNextion.setComponentValue("x0", pedal);
  delay(50);
}

void LeerPedal() {
  pedal = analogRead(PEDAL);
  while (pedal < 300) {
    pedal = analogRead(PEDAL);
    delay(20);
  }
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
  }
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
}
