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
int avance = 0;
int modo = 1; //Secuencial = 1, Paralelo = 2
int s1 = 0;
int s2 = 0;
int tiempo1 = 0;
int tiempo2 = 0;
int pasos_por_vuelta = 1600;
float pi = 3.14159;
float perimetro = 0;
double longitud = 0;
double l_m = 0;
long ciclos = 0;
long contador = 0;
int diametro = 55;

int diametroAUX = 0;
int avanceAUX = 0;
int tiempo1AUX = 0;
int tiempo2AUX = 0;
int automatico = 0;

SoftwareSerial nextion(TxD, RxD);
Nextion myNextion(nextion, 9600);

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
  perimetro = pi * diametro;//(mm)
  ciclos = readLongFromEEPROM(10);
  contador = readLongFromEEPROM(20);
  longitud = pi * diametro * ciclos;
  l_m = longitud / 1000;
  delay(300);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(BUZZER, LOW);
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
    //delay(50);
    cambio = myNextion.getComponentValue("cambio");
    while (cambio < 0) {
      cambio = myNextion.getComponentValue("cambio");
      delay(10);
    }
    if (cambio == 0) {
      myNextion.setComponentValue("x0", avance);// delay(2);
    } else if (cambio == 1) {
      //delay(100);
      avanceAUX = myNextion.getComponentValue("x0");  delay(10);
      if (avanceAUX > 1) {
        avance = avanceAUX;
        EEPROM.write(0, avance);
      }
      // delay(100);
      myNextion.setComponentValue("cambio", 0);
    }
    int bt1p1 = myNextion.getComponentValue("bt1");
    if (bt1p1 == 1) {
      myNextion.sendCommand("page 2");
      //delay(100);
    }
    int bt0p1 = myNextion.getComponentValue("bt0");//FUNCIONANDO
    if (bt0p1 == 1) {
      if (automatico == 0) {
        LeerPedal();
      }
      Main();
      if (modo == 0) {
        if (s1 == 1) {
          digitalWrite(RELAY1, LOW);
          delay(tiempo1 * 100);
          digitalWrite(RELAY1, HIGH);
          //delay(10);
        }
        if (s2 == 1) {
          digitalWrite(RELAY2, LOW);
          delay(tiempo2 * 100);
          digitalWrite(RELAY2, HIGH);
          //delay(10);
        }
      } else {
        if (tiempo1 == tiempo2) {
          if (s1 == 1) {
            digitalWrite(RELAY1, LOW);
          }
          if (s2 == 1) {
            digitalWrite(RELAY1, LOW);
          }
          delay(tiempo1 * 100);
          digitalWrite(RELAY1, HIGH);
          digitalWrite(RELAY2, HIGH);
        } else if (tiempo1 > tiempo2) {
          if (s1 == 1) {
            digitalWrite(RELAY1, LOW);
          }
          if (s2 == 1) {
            digitalWrite(RELAY2, LOW);
          }
          delay((tiempo2) * 100);
          digitalWrite(RELAY2, HIGH);
          delay((tiempo1 - tiempo2) * 100);
          digitalWrite(RELAY1, HIGH);
          //delay(10);
        } else if (tiempo1 < tiempo2) {
          if (s1 == 1) {
            digitalWrite(RELAY1, LOW);
          }
          if (s2 == 1) {
            digitalWrite(RELAY2, LOW);
          }
          delay((tiempo1) * 100);
          digitalWrite(RELAY1, HIGH);
          delay((tiempo2 - tiempo1) * 100);
          digitalWrite(RELAY2, HIGH);
          //delay(10);
        }
      }
      ciclos++;
      if (ciclos % 50 == 0) {
        writeLongIntoEEPROM(10, ciclos);
        writeLongIntoEEPROM(20, contador);
      }
    }
    int bt2p1 = myNextion.getComponentValue("bt2");
    if (bt2p1 == 1) {
      automatico = 0;
    } else {
      automatico = 1;
    }
    int bt3p1 = myNextion.getComponentValue("bt3");
    if (bt3p1 == 1) {
      s1 = 0;
    } else {
      s1 = 1;
    }
    int bt4p1 = myNextion.getComponentValue("bt4");
    if (bt4p1 == 1) {
      s2 = 0;
    } else {
      s2 = 1;
    }
  }

  if (pagina == 2) {
    //delay(50);
    cambio = myNextion.getComponentValue("cambio");
    while (cambio < 0) {
      cambio = myNextion.getComponentValue("cambio");
      delay(10);
    }
    if (cambio == 0) {
      myNextion.setComponentValue("x0", tiempo1); delay(2);
      myNextion.setComponentValue("x1", tiempo2); delay(2);
      myNextion.setComponentValue("bt0", modo); delay(2);
    } else if (cambio == 1) {
      delay(100);
      tiempo1AUX = myNextion.getComponentValue("x0");  delay(10);
      if (tiempo1AUX >= 0) {
        tiempo1 = tiempo1AUX;
        EEPROM.write(2, tiempo1);
      }
      delay(100);
      tiempo2AUX = myNextion.getComponentValue("x1");  delay(10);
      if (tiempo2AUX >= 0) {
        tiempo2 = tiempo2AUX;
        EEPROM.write(3, tiempo2);
      }
      int bt0p2 = myNextion.getComponentValue("bt0");
      if (bt0p2 >= 0) {
        modo = bt0p2;
        EEPROM.write(1, modo);
      }
      myNextion.setComponentValue("cambio", 0);
    }
    int bt1p2 = myNextion.getComponentValue("bt1");
    if (bt1p2 == 1) {
      myNextion.sendCommand("page 1");
    }
    int bt2p2 = myNextion.getComponentValue("bt2");
    if (bt2p2 == 1) {
      myNextion.sendCommand("page 3");
    }
  }

  if (pagina == 3) {
    delay(50);
    cambio = myNextion.getComponentValue("cambio");
    while (cambio < 0) {
      cambio = myNextion.getComponentValue("cambio");
      delay(10);
    }
    if (cambio == 0) {
      myNextion.setComponentValue("n0", diametro); delay(2);
    } else if (cambio == 1) {
      diametroAUX = myNextion.getComponentValue("n0");  delay(10);
      if (diametroAUX > 1) {
        diametro = diametroAUX;
        EEPROM.write(4, diametro);
      }

      int bt0p3 = myNextion.getComponentValue("bt0");
      if (bt0p3 == 1) {
        ciclos = 0;
        for (int i = 10; i < 20; i++) {
          EEPROM.write(i, 0);
        }
        myNextion.setComponentValue("bt0", 0);
      }
      int bt2p3 = myNextion.getComponentValue("bt2");
      if (bt2p3 == 1) {
        contador = 0;
        for (int i = 20; i < 30; i++) {
          EEPROM.write(i, 0);
        }
        myNextion.setComponentValue("bt2", 0);
      }
      longitud = pi * diametro * ciclos;
      myNextion.setComponentValue("cambio", 0);
    }
    longitud = pi * diametro * ciclos;
    myNextion.setComponentValue("n1", ciclos); delay(2);
    myNextion.setComponentValue("n2", contador); delay(2);
    myNextion.setComponentValue("n3", longitud / 100); delay(2);
    myNextion.setComponentValue("n4", contador / 100); delay(2);

    int bt1p2 = myNextion.getComponentValue("bt1");
    if (bt1p2 == 1) {
      myNextion.sendCommand("page 1");
    }
  }
}

void Main() {
  digitalWrite(BUZZER, HIGH);
  pot1 = analogRead(POT1);
  perimetro = pi * diametro;
  int steps = map(avance, perimetro, 0, pasos_por_vuelta, 0);
  pausa = map(pot1, 1023, 0, 500, 15);
  digitalWrite(BUZZER, LOW);
  AvanceMotor(steps);
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
