#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Nextion.h>
#include "pitches.h"
#define VOLTAJE A4
#define encoder0PinA  3
#define encoder0PinB  4
#define RELAY A5
#define LED1 7
#define LED2 9
#define LED3 10
#define LED4 11

float escala = 0.015;
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
int RPM = 10;
int RPMAUX = 0;
float constante1 = 26.5;
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
SoftwareSerial nextion(RxD, TxD);
//SoftwareSerial portOne(10, 11);
//Nextion myNextion(nextion, 57600);
Nextion myNextion(nextion, 9600);
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
  delay(100);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

void setup()
{
  if (EEPROM.read(1) < 100) {
    rango = EEPROM.read(1) * 10;
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
  if (EEPROM.read(7) < 21) {
    RPM = EEPROM.read(7);
  }
  pinMode(PinSensor, INPUT);
  pinMode(A0, INPUT);//A0 - ARDUINO D8 IN
  pinMode(A2, OUTPUT);//A2 - ARDUINO A2 OUT
  digitalWrite(A2, LOW);
  pinMode(VOLTAJE, INPUT);//A4
  pinMode(20, INPUT);//CLICK ENCODER
  pinMode(encoder0PinA, INPUT);
  digitalWrite(encoder0PinA, HIGH);       // turn on pull-up resistor
  pinMode(encoder0PinB, INPUT);
  pinMode(RELAY, OUTPUT);//ANALOG WRITE ONLY
  digitalWrite(encoder0PinB, HIGH);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(8, OUTPUT);//BUZZER
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  myNextion.sendCommand("bauds=57600");
  attachInterrupt(0, ContarPulsos, RISING); //(Interrupción 0(Pin2),función,Flanco de subida)
  attachInterrupt(1, doEncoder_Expanded, CHANGE);
  t0 = millis();
  Serial.begin(38400);
  // portOne.begin(38400);
  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  Alarma();
  digitalWrite(LED1, LOW);//bateria baja
  digitalWrite(LED2, LOW);//Falla General
  digitalWrite(LED3, LOW);//Baja fuente de gas
  digitalWrite(LED4, LOW);//Alta Presion
  myNextion.sendCommand("page 1");
  delay(100);
  myNextion.setComponentText("g0", "En Linea...");
}

void loop ()
{
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

void MedirVoltaje() {
  volts = analogRead(VOLTAJE) * escala;
  if (volts < 11.9) {
    analogWrite(RELAY, 0);
    digitalWrite(LED1, HIGH);
    digitalWrite(8, HIGH);
    ACTIVAR1 = true;
  }
  if (volts < 11) {
    analogWrite(RELAY, 0);
    digitalWrite(LED2, HIGH);
    digitalWrite(8, HIGH);
    ACTIVAR2 = true;
  }
  if (volts < 10) {
    analogWrite(A2, 0);

    digitalWrite(8, LOW);
    delay(1000);
    while (volts < 10) {
      volts = analogRead(VOLTAJE) * escala;
      EnviarDatosOFF();
      digitalWrite(LED2, HIGH);
      delay(1000);
      digitalWrite(LED2, LOW);
      delay(1000);
    }
  }
  if (volts > 13.9) {
    analogWrite(RELAY, 1023);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(8, LOW);
  }
  if (volts > 12) {
    digitalWrite(8, LOW);
    digitalWrite(LED1, LOW);
  }
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
    } else if (cambio == 1) {
      delay(100);
      rangoAUX = myNextion.getComponentValue("n0");  delay(10);
      if (rangoAUX > 1) {
        rango = rangoAUX;
        EEPROM.write(1, rango / 10);
      }
      delay(100);
      ratioAUX = myNextion.getComponentValue("n1");  delay(10);
      if (ratioAUX > 0) {
        ratio = ratioAUX;
        EEPROM.write(6, ratio);
      }
      delay(100);
      RPMAUX = myNextion.getComponentValue("n2");  delay(10);
      if (RPMAUX > 8) {
        RPM = RPMAUX;
        EEPROM.write(7, RPM);
      }
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
    CalcularPausas();
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
  tiempo1 = (60000 / RPM - delay1 - delay2) * 1000;
  t_base = tiempo1 / (1 + ratio);
  t_pausa1 = abs((t_base * (float)ratio) / ((float)rango * 46)) - constante1;
  t_pausa2 = abs((t_base) / ((float)rango * 46)) - constante1;
  //  t_pausa1 = abs((t_base * (float)ratio) / ((float)rango * 58 * 4)) - constante1;
  //t_pausa2 = abs((t_base) / ((float)rango * 58 * 4)) - constante1;
  pausa1 = t_pausa1;
  pausa2 = abs(t_pausa2);

}

void Muestreo() {
  MedirVoltaje();
  dt = millis(); //calculamos la variación de tiempo
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
  cont = frecuencia * 8;
  int voltio = volts * 10;
  char buf1[30];
  char buf2[30];
  char buf3[30];
  char buf4[30];
  char buf5[30];
  const char *first1 = "add 6,1,";
  const char *first2 = "x0.val=";
  const char *first3 = "x1.val=";//volts
  const char *first4 = "n1.val=";
  const char *first5 = "n0.val=";//Respirac RPM
  char str1[8];
  char str2[8];
  char str3[8];
  char str4[8];
  char str5[8];
  itoa(cont, str1, 10);
  itoa(frecuencia * 5, str2, 10);
  itoa(voltio, str3, 10);
  itoa(rango, str4, 10);
  itoa(RPM, str5, 10);
  //Serial.println(str);
  const char *second1 = str1;
  const char *second2 = str2;
  const char *second3 = str3;
  const char *second4 = str4;
  const char *second5 = str5;
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
  //Serial.println(buf1);
  myNextion.sendCommand(buf1);
  myNextion.sendCommand(buf2);
  myNextion.sendCommand(buf3);
  myNextion.sendCommand(buf4);
  myNextion.sendCommand(buf5);

  if (frecuencia * 4 > 100) {
    digitalWrite(8, HIGH);
    digitalWrite(LED4, HIGH);
  } else {
    digitalWrite(8, LOW);
    digitalWrite(LED4, LOW);
  }

  if (cont > 0) {//Baja Fuente de Gas
    //digitalWrite(8, HIGH);
    digitalWrite(LED3, LOW);
  } else {
    digitalWrite(LED3, HIGH);
    //digitalWrite(8, LOW);
  }

  //contador++;
  //if(contador%100==0){
  switche = analogRead(A0);
  if (switche > 100) {
    EnviarDatos();
  } else {
    digitalWrite(A2, HIGH);
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
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}


void Alarma2() {

  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void Alarma1() {

  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000; // noteDurations[thisNote];
    tone(8, melody1[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void EnviarDatos() {
  int val1 = pausa1;
  int val2 = pausa2;
  int val3 = rango / 5;
  char buf1[40];
  const char *first1 = "<1, ";
  const char *last1 = ">";
  const char *inter1 = ", ";
  char str1[8];
  char str2[8];
  char str3[8];
  char str4[8];
  char str5[8];
  itoa(val1, str1, 10);
  itoa(val2, str2, 10);
  itoa(val3, str3, 10);
  delay2 = delay1 * ratio;
  itoa(delay1, str4, 10);
  itoa(delay2, str5, 10);
  //Serial.println(str);
  strcpy(buf1, first1);
  strcat(buf1, str1);
  strcat(buf1, inter1);
  strcat(buf1, str2);
  strcat(buf1, inter1);
  strcat(buf1, str3);
  strcat(buf1, inter1);
  strcat(buf1, str4);
  strcat(buf1, inter1);
  strcat(buf1, str5);
  strcat(buf1, last1);
  Serial.println(buf1);
  //portOne.println(buf1);
}

void EnviarDatosOFF() {
  int val1 = pausa1;
  int val2 = pausa2;
  int val3 = rango / 5;
  char buf1[40];
  const char *first1 = "<0, ";
  const char *last1 = ">";
  const char *inter1 = ", ";
  char str1[8];
  char str2[8];
  char str3[8];
  char str4[8];
  char str5[8];
  itoa(val1, str1, 10);
  itoa(val2, str2, 10);
  itoa(val3, str3, 10);
  itoa(delay1, str4, 10);
  itoa(delay2, str5, 10);
  //Serial.println(str);
  strcpy(buf1, first1);
  strcat(buf1, str1);
  strcat(buf1, inter1);
  strcat(buf1, str2);
  strcat(buf1, inter1);
  strcat(buf1, str3);
  strcat(buf1, inter1);
  strcat(buf1, str4);
  strcat(buf1, inter1);
  strcat(buf1, str5);
  strcat(buf1, last1);
  Serial.println(buf1);
  //portOne.println(buf1);
}

void doEncoder_Expanded() {
  if (digitalRead(encoder0PinA) == HIGH) {   // found a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) {  // check channel B to see which way
      // encoder is turning
      encoder0Pos = encoder0Pos - 1;         // CCW
      if (rango > 50) {
        rango = rango - 1;
        EnviarDatos();
        delay(100);
      }
    }
    else {
      encoder0Pos = encoder0Pos + 1;         // CW
      if (rango < 90) {
        rango = rango + 1;
        EnviarDatos();
        delay(100);
      }
    }
  }
  else                                        // found a high-to-low on channel A
  {
    if (digitalRead(encoder0PinB) == LOW) {   // check channel B to see which way
      // encoder is turning
      encoder0Pos = encoder0Pos + 1;          // CW
      if (rango < 90) {
        rango = rango + 1;
        EnviarDatos();
        delay(100);
      }
    }
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
      if (rango > 50) {
        rango = rango - 1;
        EnviarDatos();
        delay(100);
      }
    }

  }
  //Serial.println (encoder0Pos, DEC);          // debug - remember to comment out
  // before final program run
  // you don't want serial slowing down your program if not needed
}