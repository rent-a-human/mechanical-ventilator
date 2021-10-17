#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Nextion.h>
#include "pitches.h"
#define VOLTAJEBAT A1
#define ENLACE A2
#define VOLTAJEFUE A3
#define RELAY1 A5 //desconectar cargador
#define RELAY2 A4 //CHOOSE POWER SOURCE
#define LED1 11
#define LED2 9
#define LED3 7
#define LED4 10

//462   11,75
//A    X
int P = 5;
int PAUX = 0;
int demora;
int signalvalue = 0;
int rango2 = 0;
double v1 = 0;
double v2 = 0;
int senal = 0;
double lastS = 0;
double  C1 = 1;
double  C2 = 1;
double escala = 0.025578;
boolean ACTIVAR1 = false;
boolean ACTIVAR2 = false;
volatile unsigned int encoder0Pos = 0;
// notes in the melody:
int melody[] = {
  NOTE_C4, 0, NOTE_C4, 0
};
int melody2[] = {
  NOTE_D8, 0, NOTE_C8, 0
};
int melody1[] = {
  NOTE_C1, 0, NOTE_C1, 0
};
byte dato;
int switche;
int cambio = 0;
int ratio = 1;
int ratioAUX = 0;
int RPM = 20;
int RPMAUX = 0;
float constante1 = 25.3;//26,5
long tiempo1 = 0;
long t_base = 0;
float t_pausa1 = 0;
float t_pausa2 = 0;
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  2, 2, 4, 4, 4, 4, 4, 4
};
#define RxD 5                                                                          // Usado para conexion rs232 virtual.
#define TxD 6
int cont = 0;
int gate = 0;
int contador = 0;
int pagina = 0;
float pausa = 0;
int delay1 = 500;
int delay2 = 500;
int vel1 = 35;
int vel2 = 10;
int delay1AUX = 0;
int delay2AUX = 0;
int vel1AUX = 0;
int vel2AUX = 0;
int pausa1 = 50 - vel1;
int pausa2 = 50 - vel2;
float volts = 0;
int rango = 95;
int rangoAUX = 0;
SoftwareSerial nextion(TxD, RxD);
//SoftwareSerial portOne(10, 11);
Nextion myNextion(nextion, 57600);
//Nextion myNextion(nextion, 9600);
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion = 0.5; //para convertir de frecuencia a caudal
float volumen = 0;
double dt = 0; //variación de tiempo por cada bucle
double t0 = 0; //millis() del bucle anterior
//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()
{
  NumPulsos++;  //incrementamos la variable de pulsos
}

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuecia()
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(50);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

void setup()
{
  if (EEPROM.read(1) < 100) {
    rango = EEPROM.read(1) * 10;
    rango2 = map(rango, 50, 700, 50, 500);
  }
  if (EEPROM.read(2) < 200) {
    delay1 = EEPROM.read(2) * 10;
  }
  if (EEPROM.read(3) < 200) {
    delay2 = EEPROM.read(3) * 10;
  }
  if (EEPROM.read(4) < 50) {
    vel1 = EEPROM.read(4);
    pausa1 = 50 - vel1;
  }
  if (EEPROM.read(5) < 50) {
    vel2 = EEPROM.read(5);
    pausa2 = 50 - vel2;
  }
  if (EEPROM.read(6) < 4) {
    ratio = EEPROM.read(6);
  }
  if (EEPROM.read(7) < 51) {
    RPM = EEPROM.read(7);
  }
  if (EEPROM.read(8) < 21) {
    P = EEPROM.read(8);
  }
  pinMode(PinSensor, INPUT);
  pinMode(A0, INPUT);//A0 - ARDUINO D8 IN
  pinMode(ENLACE, INPUT);//A2 MASTER IN -  SLAVE A2 OUT for senal
  pinMode(VOLTAJEBAT, INPUT);
  pinMode(VOLTAJEFUE, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY2, HIGH);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(4, OUTPUT);//BUZZER
  digitalWrite(4, HIGH);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  //myNextion.sendCommand("bauds=57600");
  attachInterrupt(0, ContarPulsos, RISING); //(Interrupción 0(Pin2),función,Flanco de subida)
  t0 = millis();
  Serial.begin(38400);
  // portOne.begin(38400);
  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  digitalWrite(LED1, HIGH);
  delay(100);
  digitalWrite(LED1, LOW);//bateria baja
  digitalWrite(LED2, HIGH);
  delay(100);
  digitalWrite(LED2, LOW);//Falla General
  digitalWrite(LED3, HIGH);
  delay(100);
  digitalWrite(LED3, LOW);//Baja fuente de gas
  digitalWrite(LED4, HIGH);
  delay(100);
  digitalWrite(LED4, LOW);//Alta Presion
  digitalWrite(4, LOW);
  Alarma();
  myNextion.sendCommand("page 1");
  delay(100);
  myNextion.setComponentText("g0", "En Linea...");
  digitalWrite(4, LOW);
}

void loop ()
{
  //LeerSensores();
  MedirVoltaje();
  ComunicarPantalla();
  /*CalcularPausas();
    Serial.print("Rango: ");
    Serial.print(rango);
    Serial.print("Ratio: ");
    Serial.print(ratio);
    Serial.print("RPM: ");
    Serial.print(RPM);
    Serial.print("PAUSA 1 ");
    Serial.print(pausa1);
    Serial.print("PAUSA 2 ");
    Serial.println(pausa2);
    EnviarDatos();
    delay(1000);
  */
}

void ComunicarPantalla() {
  pagina = myNextion.getComponentValue("pagina");
  delay(10);
  Serial.println(pagina);
  if (pagina == 0) {
    delay(100);
  }
  if (pagina == 1) {
    delay(50);
    cambio = myNextion.getComponentValue("cambio");
    while (cambio < 0) {
      cambio = myNextion.getComponentValue("cambio");
    }
    if (cambio == 0) {
      myNextion.setComponentValue("n0", rango); delay(2);
      myNextion.setComponentValue("h0", rango); delay(2);
      myNextion.setComponentValue("n1", ratio); delay(2);
      myNextion.setComponentValue("n2", RPM); delay(2);
      myNextion.setComponentValue("n3", P); delay(2);
    } else if (cambio == 1) {
      delay(100);
      rangoAUX = myNextion.getComponentValue("n0");  delay(10);
      if (rangoAUX > 1) {
        rango = rangoAUX;
        EEPROM.write(1, rango / 10);
        rango2 = map(rango, 50, 600, 50, 500);
      }
      delay(100);
      ratioAUX = myNextion.getComponentValue("n1");  delay(10);
      if (ratioAUX > 0) {
        ratio = ratioAUX;
        EEPROM.write(6, ratio);
      }
      delay(100);
      PAUX = myNextion.getComponentValue("n3");  delay(10);
      if (PAUX > 4) {
        P = PAUX;
        EEPROM.write(8, P);
      }
      CalcularTiempos();
      myNextion.setComponentValue("cambio", 0);
    }
    EnviarDatosOFF();
    digitalWrite(13, LOW);
    int bt1p1 = myNextion.getComponentValue("bt1");
    Serial.print("boton 1 ");
    Serial.println(bt1p1);
    if (bt1p1 == 1) {
      myNextion.sendCommand("page 3");
      delay(100);
    }
    int bt0p1 = myNextion.getComponentValue("bt0");
    if (bt0p1 == 1) {
      digitalWrite(13, HIGH);
      Alarma2();
      myNextion.sendCommand("page 2");
      delay(100);
    }
  }
  if (pagina == 2) {
    CalcularTiempos();
    //CalcularPausas();
    gate = digitalRead(12);
    while (true) {
      gate = digitalRead(12);
      Muestreo();
    }
    digitalWrite(A4, LOW);
    Alarma();
    EnviarDatosOFF();
    noInterrupts();
    myNextion.sendCommand("page 1");

    //}
  }
  if (pagina == 3) {
    delay(50);
    cambio = myNextion.getComponentValue("cambio");
    while (cambio < 0) {
      cambio = myNextion.getComponentValue("cambio");
    }
    if (cambio == 0) {
      myNextion.setComponentValue("n0", delay1 / 10); delay(2);
      myNextion.setComponentValue("n1", vel1); delay(2);
      myNextion.setComponentValue("n2", delay2 / 10); delay(2);
      myNextion.setComponentValue("n3", vel2); delay(2);
    } else if (cambio == 1) {
      delay(100);
      delay1AUX = myNextion.getComponentValue("n0");  delay(10);
      if (delay1AUX > 9) {
        delay1 = delay1AUX * 10;
        EEPROM.write(2, delay1AUX);
      }
      delay(100);
      delay2AUX = myNextion.getComponentValue("n2");  delay(10);
      if (delay2AUX > 9) {
        delay2 = delay2AUX * 10;
        EEPROM.write(3, delay2AUX);
      }
      delay(100);
      vel1AUX = myNextion.getComponentValue("n1");  delay(10);
      if (vel1AUX > 0) {
        vel1 = vel1AUX;
        pausa1 = 50 - vel1;
        EEPROM.write(4, vel1);
      }
      delay(100);
      vel2AUX = myNextion.getComponentValue("n3");  delay(10);
      if (vel2AUX > 0) {
        vel2 = vel2AUX;
        pausa2 = 50 - vel2;
        EEPROM.write(5, vel2);
      }
    }
    digitalWrite(13, LOW);
    int bt3p3 = myNextion.getComponentValue("bt3");
    Serial.print("boton 3 ");
    Serial.println(bt3p3);
    if (bt3p3 == 1) {

      myNextion.sendCommand("page 1");
      delay(100);
    }
  }
}

void CalcularPausas() {
  tiempo1 = (60000 / RPM  - 2 * delay2) * 1000;
  t_base = tiempo1 / (1 + ratio);
  t_pausa1 = abs((t_base * (float)ratio) / ((float)rango2 * 46)) - constante1 * 1.45;
  t_pausa2 = abs((t_base) / ((float)rango2 * 46)) - constante1 ;
  //  t_pausa1 = abs((t_base * (float)ratio) / ((float)rango * 58 * 4)) - constante1;
  //t_pausa2 = abs((t_base) / ((float)rango * 58 * 4)) - constante1;
  pausa1 = t_pausa1;
  pausa2 = abs(t_pausa2);

}

void CalcularTiempos() {
  float rango3 = rango2 / 5;
  float ajuste = -0.106;
  float k = 12.85;
  float t_micro = 58 * (float)rango3 * 6 * ((float)P + (ajuste * (float)P + k));
  demora = (t_micro / 1000) * (ratio - 1);
  float rpm_esp = 60000000 / ((t_micro * 2 + ((float)ratio - 1) * t_micro + 100000));
  RPM = rpm_esp;
  myNextion.setComponentValue("n4", demora); delay(2);
  myNextion.setComponentValue("n2", RPM); delay(2);

}

void Muestreo() {
  MedirVoltaje();
  signalvalue = analogRead(ENLACE);
  if (signalvalue > 899) {
    C2 = 1;
    //senal = -0.0045 * C1 * C1 + 0.95 * C1 + 140;
    senal = 135 * pow(C1, 0.0712);
    lastS = senal;
    C1 = C1 + 6;
  } else {

    if (C2 > 99) {
      C1 = 1;
      senal = lastS * pow(C2 - 99, -0.065);
    } else {
      senal = 135 * pow(C1, 0.0712);
    }

    C2 = C2 + 25;
    //senal = signalvalue / 20 + 140;
  }

  dt = millis(); //calculamos la 00variación de tiempo
  float frecuencia = ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
  float caudal_L_m = frecuencia / factor_conversion; //calculamos el caudal en L/m
  t0 = millis();
  volumen = volumen + caudal_L_m * (t0 - dt) / 1000; // volumen(L)=caudal(L/s)*tiempo(s)

  //-----Enviamos por el puerto serie---------------
  //Serial.print ("Caudal: ");
  //Serial.print (caudal_L_m, 3);
  //Serial.print ("L/min\tdeltaT: ");
  //Serial.print (caudal_L_m * (t0 - dt) / 1000, 3);
  //Serial.print ("\tVolumen: ");
  //Serial.print (volumen, 6);
  //Serial.println (" L");
  cont = frecuencia * 5 - 5;
  int voltio = volts * 10;

  char buf1[30];
  char buf2[30];
  char buf3[30];
  char buf4[30];
  char buf5[30];
  char buf6[30];
  char buf7[30];
  char buf8[30];
  const char *first1 = "add 6,1,";
  const char *first2 = "x0.val=";
  const char *first3 = "x1.val=";//volts
  const char *first4 = "n1.val=";
  const char *first5 = "n0.val=";//Respirac RPM
  const char *first6 = "add 6,2,";
  const char *first7 = "x2.val=";//signal value
  const char *first8 = "n2.val=";//ratio
  char str1[8];
  char str2[8];
  char str3[8];
  char str4[8];
  char str5[8];
  char str6[8];
  char str7[8];
  char str8[8];
  itoa(cont, str1, 10);
  itoa(frecuencia * 2, str2, 10);
  itoa(voltio, str3, 10);
  itoa(rango, str4, 10);
  itoa(RPM, str5, 10);
  itoa(senal, str6, 10);
  itoa(signalvalue, str7, 10);
  itoa(ratio, str8, 10);
  //Serial.println(str);
  const char *second1 = str1;
  const char *second2 = str2;
  const char *second3 = str3;
  const char *second4 = str4;
  const char *second5 = str5;
  const char *second6 = str6;
  const char *second7 = str7;
  const char *second8 = str8;
  strcpy(buf1, first1);
  strcat(buf1, second1);
  strcpy(buf2, first2);
  strcat(buf2, second2);
  strcpy(buf3, first3);
  strcat(buf3, second3);
  strcpy(buf4, first4);
  strcat(buf4, second4);
  strcpy(buf5, first5);
  strcat(buf5, second5);
  strcpy(buf6, first6);
  strcat(buf6, second6);
  strcpy(buf7, first7);
  strcat(buf7, second7);
  strcpy(buf8, first8);
  strcat(buf8, second8);
  //Serial.println(buf1);
  myNextion.sendCommand(buf1);
  myNextion.sendCommand(buf2);
  myNextion.sendCommand(buf3);
  myNextion.sendCommand(buf4);
  myNextion.sendCommand(buf5);
  myNextion.sendCommand(buf6);
  myNextion.sendCommand(buf7);
  myNextion.sendCommand(buf8);
  if (frecuencia * 3 > 100) {
    interrupts();

    digitalWrite(4, HIGH);
    digitalWrite(LED4, HIGH);
    delay(1000);
    noInterrupts();
  } else {
    digitalWrite(4, LOW);
    digitalWrite(LED4, LOW);
  }

  if (cont > 0) {//Baja Fuente de Gas
    //digitalWrite(4, HIGH);
    digitalWrite(LED3, LOW);
  } else {
    digitalWrite(LED3, HIGH);
    //digitalWrite(4, LOW);
  }

  //contador++;
  //if(contador%100==0){
  switche = analogRead(A0);
  if (switche > 100) {
    EnviarDatos();
  } else {
    //digitalWrite(ENLACE, HIGH);
  }

  if (ACTIVAR1 ==  true) {
    Alarma1();
  }

  if (ACTIVAR2 == true) {
    digitalWrite(LED2, HIGH);
    //while (true){

    //}

  }

  //}

}

void Alarma() {

  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(4, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
}


void Alarma2() {

  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(4, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
}

void Alarma1() {

  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000; // noteDurations[thisNote];
    tone(4, melody1[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
}

void EnviarDatos() {
  int val1 = P;
  int val2 = rango2 / 5;
  int val3 = demora;
  char buf1[40];
  const char *first1 = "<1, ";
  const char *last1 = ">";
  const char *inter1 = ", ";
  char str1[8];
  char str2[8];
  char str3[8];
  itoa(val1, str1, 10);
  itoa(val2, str2, 10);
  itoa(val3, str3, 10);
  //delay2 = delay1 * ratio;
  //Serial.println(str);
  strcpy(buf1, first1);
  strcat(buf1, str1);
  strcat(buf1, inter1);
  strcat(buf1, str2);
  strcat(buf1, inter1);
  strcat(buf1, str3);
  strcat(buf1, last1);
  Serial.println(buf1);
  //portOne.println(buf1);
}

void EnviarDatosOFF() {
  int val1 = P;
  int val2 = rango2 / 5;
  int val3 = demora;
  char buf1[40];
  const char *first1 = "<0, ";
  const char *last1 = ">";
  const char *inter1 = ", ";
  char str1[8];
  char str2[8];
  char str3[8];
  itoa(val1, str1, 10);
  itoa(val2, str2, 10);
  itoa(val3, str3, 10);
  //delay2 = delay1 * ratio;
  //Serial.println(str);
  strcpy(buf1, first1);
  strcat(buf1, str1);
  strcat(buf1, inter1);
  strcat(buf1, str2);
  strcat(buf1, inter1);
  strcat(buf1, str3);
  strcat(buf1, last1);
  Serial.println(buf1);
  //portOne.println(buf1);
}

void LeerSensores() {
  int v1 = analogRead(VOLTAJEBAT);
  int v2 = analogRead(VOLTAJEFUE);
  Serial.print("vbat ");
  Serial.print(v1);
  Serial.print("vfue ");
  Serial.println(v2);
  delay(100);
}
void MedirVoltaje() {
  v1 = analogRead(VOLTAJEBAT) * escala;
  v2 = analogRead(VOLTAJEFUE) * escala;
  //Serial.print("Volts1 ");
  //Serial.print(v1);
  //Serial.print("Volts2 ");
  //Serial.println(v2);
  volts = v1;
  if (v2 < 5) {
    digitalWrite(RELAY2, HIGH);
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(RELAY2, LOW);
    digitalWrite(LED2, LOW);
  }
  if (v1 < 11.9) {
    digitalWrite(RELAY1, HIGH);
    digitalWrite(LED1, HIGH);
    //digitalWrite(4, HIGH);
    //delay(1000);
    //digitalWrite(4, LOW);
  }
  if (v1 < 11) {
    digitalWrite(RELAY1, HIGH);
    digitalWrite(LED2, HIGH);
    //digitalWrite(4, HIGH);
    //delay(1000);
    //digitalWrite(4, LOW);
  }
  if (v1 < 10) {
    analogWrite(A1, 0);
    digitalWrite(4, LOW);
    delay(1000);
    while (v1 < 10) {
      v1 = analogRead(VOLTAJEBAT) * escala;
      EnviarDatosOFF();
      digitalWrite(LED2, HIGH);
      delay(1000);
      digitalWrite(LED2, LOW);
      delay(1000);
    }
  }
  if (v1 > 13.9) {
    digitalWrite(RELAY1, HIGH);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(4, LOW);
  }
  if (v1 > 12) {
    digitalWrite(4, LOW);
    digitalWrite(LED1, LOW);
  }


}
