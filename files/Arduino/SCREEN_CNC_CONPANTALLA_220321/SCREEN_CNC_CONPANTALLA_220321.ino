#include <EEPROM.h>
#include <math.h>
#include <Nextion.h>

#define BANDEJA1 22
#define BANDEJA2 24
#define BANDEJA3 26
#define BANDEJA4 28

#define CARRO1 30
#define CARRO2 32
#define CARRO3 34
#define CARRO4 36

#define AIRE 13

int pnormal = 160;
int plenta = 180;
int prapida = 140;
int GIRO = 0;
int GIROAUX = 0;
int ESPERA_GIRO = 1;
int ESPERA_GIROAUX = 0;
int ciclos_reinicioAUX = 0;
int VUELTAS = 0;
int seteo = 0;
int ReinicioCiclos = 0;
int ReinicioCiclosAUX = 0;
#define nextion Serial2
Nextion myNextion(nextion, 19200);

int subirtinta = 0;
int bajartinta = 0;
int conf = 0;
int bt1p2 = 0;
int bt2p2 = 0;
int bt2p1 = 0;
int bt1p3 = 0;
int bt2p3 = 0;
int bt1p4 = 0;
int bt2p4 = 0;
int tiempoimprimir = 80;

int unciclo = 0;
int unciclocarro = 0;
int va1p2 = 0;
int va1p3 = 0;
int va1p4 = 0;
float indice = 600; //pausa=indice*paso^exponente
float exponente = -0.25;
float pausaAUX = 120;

int velocidadAUX = 0;
int diametroAUX = 0;
int ANCHOAUX = 0;
int anguloAUX = 0;
int centroAUX = 0;
int BSP0AUX = 0;
int BSP1AUX = 0;
int CSP0AUX = 0;
int CSP1AUX = 0;
int TSP0AUX = 0;
int TSP1AUX = 0;
int TSP2AUX = 0;
int OFFSETAUX = 0;


int p0 = 300;
int p1 = 150;
int p2 = 80;
int p3 = 40;
int p4 = 20;
float pausa = 160;
int pause = 0;
int pausamax = 160;
int pausainicial = 160;
int contador = 0;
int imprimir = 0;
int automatico = 0;
int multicolor = 0;
int reset = 0;
int va0 = 0;
int va1 = 0;
int velocidad = 0;
int ciclos = 0;
int valvula = 0;

int reiniciar = 1;
int diametro = 50;
float angulo = 0;
int anchoMax = 290;
int ANCHO = 120;
int centro = 150;
float BSP0 = 20;
float BSP1 = 280;
float CSP0 = 20;
float CSP1 = 90;
float TSP0 = 80; //recoger
float TSP1 = 10; //imprimir
float TSP2 = 7; //entintar
int OFFSET_DER = 20;
int OFFSET_IZQ = 20;
double APP1 = 19; //pasos por milimetro
double APP2 = diametro / 127.32366;
double frecuencia = APP2 * APP1;

double tsp0 = TSP0 * APP1; //UNIDADES EN PASOS
double tsp1 = TSP1 * APP1;
double tsp2 = TSP2 * APP1;
double bsp0 = BSP0 * APP1;
double bsp1 = BSP1 * APP1;
double csp0 = CSP0 * APP1;
double csp1 = CSP1 * APP1;
double offset_der = OFFSET_DER * APP1;
double offset_izq = OFFSET_IZQ * APP1;
double ancho = ANCHO / APP2;

int pedal = 0;

int steps = 0;

int sensor_bandeja = 0;
int sensor_carro = 0;
int sensor_tinta = 0;
int sensor_envase = 0;

long PAB = 0; //Paso Actual Bandeja
long PAC = 0;
long PAT = 0;
long PAE = 0;
int ciclos_reinicio = 0;
long MarcaColor = 0;

int modo = 0;

void setup() {
  Serial.begin(19200);;
  Serial.print("Inicio de programa ");
  //Motor Bandeja
  pinMode(BANDEJA1, OUTPUT);
  pinMode(BANDEJA2, OUTPUT);
  pinMode(BANDEJA3, OUTPUT);
  pinMode(BANDEJA4, OUTPUT);
  //Motor Carro
  pinMode(CARRO1, OUTPUT);
  pinMode(CARRO2, OUTPUT);
  pinMode(CARRO3, OUTPUT);
  pinMode(CARRO4, OUTPUT);
  //Motor Tinta
  pinMode(38, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(44, OUTPUT);
  //Motor Envase
  pinMode(46, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(52, OUTPUT);
  //Sensores:A0 Bandeja, A1 Carro, A2 Tinta, A3 Envase, A4 Pedal
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(12, OUTPUT);
  pinMode(AIRE, OUTPUT);
  digitalWrite(AIRE, HIGH);
  delay(1000);
  if (myNextion.init()) {
    Serial.println(F("Nextion Ready..."));
  } else {
    Serial.println(F("Nextion not responding..."));
  }
  delay(1000);
  //myNextion.sendCommand("bauds=19200");
  myNextion.sendCommand("page 1");
  delay(500);
  /*myNextion.sendCommand("page1.n0.val=12");
    delay(600);
    modo=myNextion.getComponentValue("n0");
    Serial.println(modo);
  */
  velocidad = EEPROM.read(0);
  diametro = EEPROM.read(1);
  ANCHO = EEPROM.read(2);
  angulo = EEPROM.read(3);
  angulo = 2 * angulo;
  CSP0 = EEPROM.read(5);
  CSP1 = EEPROM.read(6);
  TSP0 = EEPROM.read(7);
  TSP1 = EEPROM.read(8);
  TSP2 = EEPROM.read(9);
  OFFSET_DER = EEPROM.read(10);
  BSP0 = EEPROM.read(11);
  BSP1 = EEPROM.read(12);
  ReinicioCiclos = EEPROM.read(13);
  ciclos_reinicio = EEPROM.read(14);
  GIRO = EEPROM.read(15);
  ESPERA_GIRO = EEPROM.read(16);
  BSP1 = 20 + BSP1;

  /////// PARAMETROS DE IMPRESION  ////////
  /*
    diametro = 67;// Diametro medio del envase
    CSP0 = 35;//Posicion inferior portaenvase
    CSP1 = 62;//Posicion de impresion portaenvase
    TSP0 = 92;//Posicion inicial Tinta
    TSP1 = 6;// cuantos milimetros baja el escobillin para imprimir
    TSP2 = 5;// cuantos milimetros baja el escobillin para entintar
    OFFSET_DER = 18;//milimetros para recoger la tinta
    BSP0 = 20;//Punto de inicio impresion, minimo 20
    BSP1 = 275;//Punto final impresion, maximo 270
    angulo=90;//Angulo de giro para sensor color
    ciclos_reinicio=5;// Cada cuantos ciclos hago el reinicio de todos los actuadores
    multicolor=1; // Modo multicolor: 1 activado, 0 desactivado
    automatico=0;
    pause=2; //pausa arranque imprimir en segundos
    // PARAMETROS DE VELOCIDAD
    p0=55;
    p1=30;
    p2=20;
  */
  /////// FIN PARAMETROS    /////////////
  CalcularVariables();

  Serial.print(" FIN Inicio de programa ");
  reiniciar = 1;
}

void CalcularVariables() {
  p0 = 6600 / velocidad;
  p1 = 5600 / velocidad;
  p2 = 4600 / velocidad;
  p3 = 3600 / velocidad;
  p4 = 2600 / velocidad;
  ANCHO = BSP1 - BSP0;
  pausamax = velocidad / 100 * pausamax;
  pausainicial = pausa;
  APP2 = diametro / 127.32366;
  frecuencia = APP2 * APP1;
  OFFSET_IZQ = OFFSET_DER;
  tsp0 = TSP0 * APP1; //UNIDADES EN PASOS
  tsp1 = TSP1 * APP1;
  tsp2 = TSP2 * APP1;
  bsp0 = BSP0 * APP1;
  bsp1 = BSP1 * APP1;
  csp0 = CSP0 * APP1;
  csp1 = CSP1 * APP1;
  offset_der = OFFSET_DER * APP1;
  offset_izq = OFFSET_IZQ * APP1;
  ancho = ANCHO / APP2;
}

void loop() {
  delay(10);
  Serial.print("REiniciar: ");
  if (reiniciar = !1) {
    reiniciar = ciclos % ciclos_reinicio + 1;
    Serial.println(reiniciar);
  }
  //IMPRIMIR();
  Configurar();
  myNextion.setComponentText("g0", "En Linea...");
}

void LeerMarca() {
  MarcaColor = analogRead(A3);
  while (MarcaColor < 300 && VUELTAS < 1000) {
    if (GIRO == 0) {
      AvanceEnvase(1);
    } else {
      RetroEnvase(1);
    }
    delay(ESPERA_GIRO);
    MarcaColor = analogRead(A3);
    VUELTAS++;
    if (VUELTAS > 998) {
      bt2p1 = myNextion.getComponentValue("bt2");
      while (bt2p1 < 0) {
        bt2p1 = myNextion.getComponentValue("bt2");
      }
      if (bt2p1 == 1) {
        delay(200);
        myNextion.setComponentText("g0", "Corregir Marca");
        delay(1000);
        LeerPedal();
        VUELTAS = 0;
      }
    }
  }
  VUELTAS = 0;
  delay(100);
  for (int i = 0; i < (angulo * 400 / 360); i++) {
    RetroEnvase(1);
    delay(1);
  }
}

void Configurar() {
  delay(100);
  va0 = myNextion.getComponentValue("va0");
  delay(200);
  while (va0 < 0) {
    va0 = myNextion.getComponentValue("va0");
    delay(10);
    Serial.print("Va0 ERR :  ");
    Serial.println(va0);
    //myNextion.setComponentText("g0","-1 err");
  }
  Serial.print("Va0 :  ");
  Serial.println(va0);
  // Serial.print("Pagina: ");
  //Serial.println(va0);
  switch (va0) {
    case 1://BOTONES DE PAG 1
      {
        delay(100);
        bt2p1 = myNextion.getComponentValue("bt2");
        while (bt2p1 < 0) {
          bt2p1 = myNextion.getComponentValue("bt2");
        }
        if (bt2p1 == 0) {
          va1 = myNextion.getComponentValue("va1");
          while (va1 < 0) {
            va1 = myNextion.getComponentValue("va1");
          }
          delay(20);
          // Serial.print("va1: ");
          //Serial.println(va1);
          if (va1 == 0) {
            myNextion.setComponentValue("n0", contador);
          } else if (va1 == 1) {
            contador = 0;
            myNextion.setComponentValue("va1", 0);
          }
          delay(100);
          reset = myNextion.getComponentValue("bt3");
          if (reset == 1) {
            myNextion.setComponentText("g0", "Reiniciando...");
            ReiniciarTodos();
            AvanceTinta(tsp0);
            AvanceCarro(csp0);
            AvanceBandeja(bsp0 - offset_der);
            AvanceTinta(tsp2);
            myNextion.setComponentValue("bt3", 0);
            delay(2);
            myNextion.setComponentText("g0", "Reiniciado!");
            delay(1000);
            reiniciar = 1;
          }
          delay(100);
          subirtinta = myNextion.getComponentValue("bt7");
          if (subirtinta == 1) {
            Serial.println("Reiniciando Tinta");
            ReiniciarTinta();
            reiniciar = 1;
            myNextion.setComponentValue("bt7", 0);
            subirtinta = 0;
          }
          delay(100);
          bajartinta = myNextion.getComponentValue("bt8");
          if (bajartinta == 1) {
            if (PAT < 200) {
              while (PAT < tsp0) {
                AvanceTinta(1);
              }
              reiniciar = 1;
            }
            myNextion.setComponentValue("bt8", 0);
          }

          delay(10);
          multicolor = myNextion.getComponentValue("bt0");
          while (multicolor < 0) {
            multicolor = myNextion.getComponentValue("bt0");
          }
          digitalWrite(12, !multicolor);
          delay(10);
          valvula = myNextion.getComponentValue("bt5");
          while (valvula < 0) {
            valvula = myNextion.getComponentValue("bt5");
          }
          digitalWrite(AIRE, !valvula);
          delay(10);
          automatico = myNextion.getComponentValue("bt1");
          while (automatico < 0) {
            automatico = myNextion.getComponentValue("bt1");
          }

          if (automatico == 1) {
            pause = myNextion.getComponentValue("n1");
            if (pause < 0) {
              pause = 0;
            }
          }

        } else if (bt2p1 == 1) { // VAMOS A IMPRIMIR
          imprimir = 1;
          Serial.print("Imprimir: ");
          Serial.println(imprimir);
          myNextion.setComponentText("g0", "Imprimiendo...");
          Serial.println("Imprimiendo... ");
          IMPRIMIR();
          delay(20);
          myNextion.setComponentText("g0", "Impreso!");
          delay(20);
        }
        delay(100);
        conf = myNextion.getComponentValue("bt4");
        if (conf == 1) {
          myNextion.sendCommand("page 2");
          delay(100);
        }
        break;

      }


    case 2://Pag 2
      delay(200);
      Serial.print("En pag 2 ");
      va1p2 = myNextion.getComponentValue("va1");
      while (va1p2 < 0) {
        va1p2 = myNextion.getComponentValue("va1");
        delay(10);
      }
      Serial.println(va1p2);
      if (va1p2 == 0) {
        myNextion.setComponentValue("h0", BSP0); delay(10);
        myNextion.setComponentValue("n0", BSP0); delay(10);
        myNextion.setComponentValue("h1", BSP1); delay(10);
        myNextion.setComponentValue("n1", BSP1); delay(10);
        myNextion.setComponentValue("n2", BSP1 - BSP0); delay(10);
      } else if (va1p2 == 1) {
        delay(100);
        BSP0AUX = myNextion.getComponentValue("n0"); delay(2);
        BSP1AUX = myNextion.getComponentValue("n1"); delay(2);
        if (BSP0AUX > 0) {
          BSP0 = BSP0AUX;
          EEPROM.write(11, BSP0);
        }
        if (BSP1AUX > 0) {
          BSP1 = BSP1AUX;
          EEPROM.write(12, BSP1 - 20);
        }
        CalcularVariables();
      }
      delay(100);
      unciclo = myNextion.getComponentValue("bt0");
      if (unciclo == 1) {
        Serial.println("Imprimiendo un ciclo bandeja");
        CalcularVariables();
        IMPRIMIRUNCICLOBANDEJA();
        myNextion.setComponentValue("bt0", 0);
        Serial.println("Imprimir un ciclo ok bandeja");
      }
      delay(100);
      bt1p2 = myNextion.getComponentValue("bt1");
      if (bt1p2 == 1) {
        myNextion.sendCommand("page 1");
      }
      delay(100);
      bt2p2 = myNextion.getComponentValue("bt2");
      if (bt2p2 == 1) {
        myNextion.sendCommand("page 3");
      }
      break;

    case 3:
      delay(100);
      va1p3 = myNextion.getComponentValue("va1");
      Serial.print("Pagina 3 va1: ");
      Serial.println(va1p3);
      Serial.print("CSPO: ");
      Serial.println(CSP0);
      if (va1p3 == 0) {
        myNextion.setComponentValue("h0", CSP0); delay(2);
        myNextion.setComponentValue("n0", CSP0); delay(2);
        myNextion.setComponentValue("h1", CSP1); delay(2);
        myNextion.setComponentValue("n1", CSP1); delay(2);
        myNextion.setComponentValue("h2", 100 - TSP0); delay(2);
        myNextion.setComponentValue("n2", TSP0); delay(2);
        myNextion.setComponentValue("h3", 21 - TSP1); delay(2);
        myNextion.setComponentValue("n3", TSP1); delay(2);
        myNextion.setComponentValue("h4", 21 - TSP2); delay(2);
        myNextion.setComponentValue("n4", TSP2); delay(2);
        myNextion.setComponentValue("h5", OFFSET_DER); delay(2);
        myNextion.setComponentValue("n5", OFFSET_DER); delay(2);
        delay(500);
      } else if (va1p3 == 1) {
        delay(100);
        CSP0AUX = myNextion.getComponentValue("n0"); delay(2);
        CSP1AUX = myNextion.getComponentValue("n1"); delay(2);
        TSP0AUX = myNextion.getComponentValue("n2"); delay(2);
        TSP1AUX = myNextion.getComponentValue("n3"); delay(2);
        TSP2AUX = myNextion.getComponentValue("n4"); delay(2);
        OFFSETAUX = myNextion.getComponentValue("n5"); delay(2);
        if (CSP0AUX > 0) {
          CSP0 = CSP0AUX;
          EEPROM.write(5, CSP0);
        }
        if (CSP1AUX > 0) {
          CSP1 = CSP1AUX;
          EEPROM.write(6, CSP1);
        }
        if (TSP0AUX > 0) {
          TSP0 = TSP0AUX;
          EEPROM.write(7, TSP0);
        }
        if (TSP1AUX > 0) {
          TSP1 = TSP1AUX;
          EEPROM.write(8, TSP1);
        }
        if (TSP2AUX > 0) {
          TSP2 = TSP2AUX;
          EEPROM.write(9, TSP2);
        }
        if (OFFSETAUX > 0) {
          OFFSET_DER = OFFSETAUX;
          EEPROM.write(10, OFFSET_DER);
        }
      }
      CalcularVariables();
      delay(100);
      unciclocarro = myNextion.getComponentValue("bt3");
      if (unciclocarro == 1) {

        CalcularVariables();
        Serial.println("Imprimiendo un ciclo carro");
        ReiniciarCarro();
        while (PAC < csp0) {
          AvanceCarro(1);
        }
        delay(1000);
        while (PAC < csp1) {
          AvanceCarro(1);
        }
        delay(1000);
        while (PAC > csp0) {
          RetroCarro(1);
        }
        reiniciar = 1;
        myNextion.setComponentValue("bt3", 0);
        Serial.println("Imprimir un ciclo ok carro");
      }
      delay(100);
      unciclo = myNextion.getComponentValue("bt0");
      if (unciclo == 1) {
        CalcularVariables();
        Serial.println("Imprimiendo un ciclo Tinta");
        ReiniciarTinta();
        while (PAT < tsp0) {
          AvanceTinta(1);
        }
        delay(1000);
        while (PAT < (tsp0 + tsp1)) {
          AvanceTinta(1);
        }
        delay(1000);
        while (PAT > tsp0) {
          RetroTinta(1);
        }
        reiniciar = 1;
        myNextion.setComponentValue("bt0", 0);
        Serial.println("Imprimir un ciclo ok tinta");
      }

      bt1p3 = myNextion.getComponentValue("bt1");
      if (bt1p3 == 1) {
        if (CSP0AUX > 0) {
          CSP0 = CSP0AUX;
          EEPROM.write(5, CSP0);
        }
        if (CSP1AUX > 0) {
          CSP1 = CSP1AUX;
          EEPROM.write(6, CSP1);
        }
        if (TSP0AUX > 0) {
          TSP0 = TSP0AUX;
          EEPROM.write(7, TSP0);
        }
        if (TSP1AUX > 0) {
          TSP1 = TSP1AUX;
          EEPROM.write(8, TSP1);
        }
        if (TSP2AUX > 0) {
          TSP2 = TSP2AUX;
          EEPROM.write(9, TSP2);
        }
        if (OFFSETAUX > 0) {
          OFFSET_DER = OFFSETAUX;
          EEPROM.write(10, OFFSET_DER);
        }
        CalcularVariables();
        myNextion.sendCommand("page 2");
      }

      bt2p3 = myNextion.getComponentValue("bt2");
      if (bt2p3 == 1) {
        if (CSP0AUX > 0) {
          CSP0 = CSP0AUX;
          EEPROM.write(5, CSP0);
        }
        if (CSP1AUX > 0) {
          CSP1 = CSP1AUX;
          EEPROM.write(6, CSP1);
        }
        if (TSP0AUX > 0) {
          TSP0 = TSP0AUX;
          EEPROM.write(7, TSP0);
        }
        if (TSP1AUX > 0) {
          TSP1 = TSP1AUX;
          EEPROM.write(8, TSP1);
        }
        if (TSP2AUX > 0) {
          TSP2 = TSP2AUX;
          EEPROM.write(9, TSP2);
        }
        if (OFFSETAUX > 0) {
          OFFSET_DER = OFFSETAUX;
          EEPROM.write(10, OFFSET_DER);
        }
        CalcularVariables();
        myNextion.sendCommand("page 4");
      }
      break;

    case 4: //PAGINA 4
      delay(100);
      va1p4 = myNextion.getComponentValue("va1");
      Serial.print("Pagina 4 va1: ");
      Serial.println(va1p4);
      if (va1p4 == 0) {
        myNextion.setComponentValue("h0", diametro); delay(2);
        myNextion.setComponentValue("n0", diametro); delay(2);
        myNextion.setComponentValue("h1", velocidad); delay(2);
        myNextion.setComponentValue("n1", velocidad); delay(2);
        myNextion.setComponentValue("h2", angulo); delay(2);
        myNextion.setComponentValue("n2", angulo); delay(2);
        myNextion.setComponentValue("n3", ReinicioCiclos); delay(2);
        myNextion.setComponentValue("n4", ciclos_reinicio); delay(2);
        myNextion.setComponentValue("bt3", GIRO); delay(2);
        myNextion.setComponentValue("n5", ESPERA_GIRO); delay(2);
      } else if (va1p4 == 1) {
        delay(100);
        diametroAUX = myNextion.getComponentValue("n0"); delay(2);
        velocidadAUX = myNextion.getComponentValue("n1"); delay(2);
        anguloAUX = myNextion.getComponentValue("n2"); delay(2);
        ReinicioCiclosAUX = myNextion.getComponentValue("n3"); delay(2);
        ciclos_reinicioAUX = myNextion.getComponentValue("n4"); delay(100);
        GIROAUX = myNextion.getComponentValue("bt3"); delay(2);
        ESPERA_GIROAUX = myNextion.getComponentValue("n5"); delay(2);
        if (velocidadAUX > 0) {
          velocidad = velocidadAUX;
          EEPROM.write(0, velocidad);
          delay(2);
          Serial.println(velocidad);
        }
        if (diametroAUX > 0) {
          diametro = diametroAUX;
          EEPROM.write(1, diametro);
          delay(2);
        }
        if (anguloAUX > 0) {
          angulo = anguloAUX;
          EEPROM.write(3, angulo / 2);
        }
        if (ReinicioCiclosAUX > 0) {
          ReinicioCiclos = ReinicioCiclosAUX;
          EEPROM.write(13, ReinicioCiclos);
        }
        if (ciclos_reinicioAUX > 0) {
          ciclos_reinicio = ciclos_reinicioAUX;
          EEPROM.write(14, ciclos_reinicio);
        }
        if (GIROAUX >= 0) {
          GIRO = GIROAUX;
          EEPROM.write(15, GIRO);
        }
        if (ESPERA_GIROAUX > 0) {
          ESPERA_GIRO = ESPERA_GIROAUX;
          EEPROM.write(16, ESPERA_GIRO);
        }
      }
      delay(100);
      unciclo = myNextion.getComponentValue("bt0");
      if (unciclo == 1) {
        CalcularVariables();
        IMPRIMIRUNCICLO();
        myNextion.setComponentValue("bt0", 0);
        Serial.println("Imprimir un ciclo ok");
      }
      delay(100);
      bt1p4 = myNextion.getComponentValue("bt1");
      if (bt1p4 == 1) {
        CalcularVariables();
        myNextion.sendCommand("page 3");
      }
      delay(100);
      bt2p4 = myNextion.getComponentValue("bt2");
      if (bt2p4 == 1) {
        CalcularVariables();
        myNextion.sendCommand("page 1");
      }
      break;

    default:
      myNextion.setComponentText("g0", "Error!");
  }
}


void IMPRIMIRUNCICLOBANDEJA() {
  ReiniciarBandeja();
  AvanceBandeja(bsp0 - offset_der);
  delay(1000);
  AvanceBandeja(offset_der);
  delay(1000);
  AvanceBandeja(bsp1 - bsp0);
  delay(1000);
  AvanceBandeja(offset_der);
  delay(1000);
  RetroBandeja(offset_der);
  delay(1000);
  RetroBandeja(bsp1 - bsp0); // llegaria hasta bsp0
  delay(1000);
  RetroBandeja(offset_der);
  delay(1000);
  ReiniciarBandeja();
  reiniciar = 1;

}

void IMPRIMIRUNCICLO() {
  CalcularVariables();

  ReinicioSincrono();
  pausa = pausa / 2;
  while (PAT < tsp0 && PAC < csp0) {
    AvanceTinta(1);
    AvanceCarro(1);
  }
  pausa = 2 * pausa;
  while (PAT < tsp0) {
    AvanceTinta(1);
  }
  while (PAC < csp0) {
    AvanceCarro(1);
  }

  while (PAT > tsp0) {
    RetroTinta(1);
  }
  while (PAC > csp0) {
    RetroCarro(1);
  }
  ReiniciarBandeja();
  AvanceBandeja(bsp0 - offset_der); //bsp0 siempre mayor a 20mm, offset_der siempre menor a 20
  AvanceTinta(tsp2);//tsp2 recoger tinta siempre menor a 20mm

  AvanceCarro(csp1 - csp0);
  AvanceBandeja(offset_der - (tsp1 - tsp2));
  ASBT(tsp1 - tsp2);
  ASBE(ancho);

  ASBTC(tsp1);
  ASBC(offset_izq - (tsp1));

  ASCT(tsp2);
  RSBC(PAC - csp0);
  RetrocederBandeja(PAB - bsp0);
  RSBT(tsp2);
  RetroBandeja(offset_der - (tsp2));
  reiniciar = 1;
}

void IMPRIMIR() {
  pausa = pnormal;
  Serial.println("Imprimiendo...");
  CalcularVariables();
  reiniciar = ciclos % ciclos_reinicio + 1;
  if (reiniciar == 1 || ciclos == 0) {
    ReinicioSincrono();
    ReiniciarTodos();
    Serial.println("Reiniciado");
    reiniciar = 0;
  }
  pausa = plenta;
  while (PAT < tsp0 && PAC < csp0) {
    AvanceTinta(1);
    AvanceCarro(1);
  }

  while (PAT < tsp0) {
    AvanceTinta(1);
  }
  while (PAC < csp0) {
    AvanceCarro(1);
  }

  while (PAT > tsp0) {
    RetroTinta(1);
  }
  while (PAC >= csp0) {
    RetroCarro(1);
  }
  if ((ciclos % ReinicioCiclos) == 0) {
    ReiniciarBandeja();
    AvanceBandeja(bsp0 - offset_der);
  }

  while (PAB <= (bsp0 - offset_der)) {
    AvanceBandeja(1);
  }

  while (PAB > (bsp0 - offset_der)) {
    RetroBandeja(1);
  }
  pausa = pnormal;
  //AvanceBandeja(bsp0 - offset_der); //bsp0 siempre mayor a 20mm, offset_der siempre menor a 20
  AvanceTinta(tsp2);//tsp2 recoger tinta siempre menor a 20mm

  if (automatico == 0) {
    LeerPedal();
  } else {
    if (pause >= 0 && pause < 7) {
      delay(1000 * pause);
    }
  }
  delay(100);
  bt2p1 = myNextion.getComponentValue("bt2");
  while (bt2p1 < 0) {
    bt2p1 = myNextion.getComponentValue("bt2");
  }
  if (valvula == 1) {
    digitalWrite(AIRE, LOW);
    delay(400);
  }
  if (multicolor == 1) {

    LeerMarca();
  }
  if (bt2p1 == 1) {
    AvanceCarro(csp1 - csp0);
    AvanceBandeja(offset_der - (tsp1 - tsp2));
    ASBT(tsp1 - tsp2);
    ASBE(ancho);
    delay(tiempoimprimir);
    ASBTC(tsp1);//PARA QUE TINTA QUEDE EN TSP0, BANDEJA EN BSP1+TSP1
    ASBC(offset_izq - (tsp1));
    delay(tiempoimprimir);
    ASCT(tsp2);
    digitalWrite(AIRE, HIGH);
    delay(tiempoimprimir);
    if (PAC > csp0) {
      RSBC(PAC - csp0);
    }
    //delay(tiempoimprimir);
    pausa = pnormal;
    RetrocederBandeja(PAB - bsp0);
    RSBT(tsp2);
    pausa = plenta;
    RetroBandeja(offset_der - (tsp2));
    pausa = pnormal;
    ciclos++;
    contador++;
    myNextion.setComponentValue("n0", contador);

  } else {
    ReinicioSincrono();
    reiniciar = 1;
  }
}


void LeerPedal() {
  pedal = analogRead(A4);
  while (pedal < 400) {
    pedal = analogRead(A4);
    Serial.println("Esperando pedal");
    myNextion.setComponentText("g0", "Esperando Pedal");
    delay(50);
    bt2p1 = myNextion.getComponentValue("bt2");
    if (bt2p1 == 0) {
      pedal = 500;
    }
    //myNextion.setComponentText("g0","Esperando pedal");
    delay(50);
  }
  pedal = 0;
  myNextion.setComponentText("g0", "Imprimiendo");
}

void RSBT(int steps) {
  for (int i = 0; i < steps; i++) {
    RetroBandeja(1);
    RetroTinta(1);
  }
}

void RSBC(int steps) {
  for (int i = 0; i < steps; i++) {
    RetroBandeja(1);
    RetroCarro(1);
  }
}

void ASCT(int steps) {
  if (steps < (PAC - csp0)) {
    for (int i = 0; i < steps; i++) {
      AvanceTinta(1);
      RetroCarro(1);
    }
  } else {
    for (int i = 0; i < (PAC - csp0); i++) {
      AvanceTinta(1);
      RetroCarro(1);
    }
    for (int i = 0; i < steps - (PAC - csp0); i++) {
      AvanceTinta(1);
    }
  }
}

void ASBC(int steps) {
  if (steps < (PAC - csp0)) {
    pausa = pausa / 2;
    for (int i = 0; i < steps; i++) {
      AvanceBandeja(1);
      RetroCarro(1);
    }
    pausa = pnormal;
  } else {
    for (int i = 0; i < (PAC - csp0); i++) {
      AvanceBandeja(1);
      RetroCarro(1);
    }
    for (int i = 0; i < steps - (PAC - csp0); i++) {
      AvanceBandeja(1);
    }
  }
}

void ASBTC(int steps) {
  if (tsp1 < (csp1 - csp0)) {
    for (int i = 0; i < tsp1; i++) {
      AvanceBandeja(1);
      RetroTinta(1);
      RetroCarro(1);
    }
    pausa = pnormal;
  } else {
    for (int i = 0; i < (csp1 - csp0); i++) {
      AvanceBandeja(1);
      RetroTinta(1);
      RetroCarro(1);
    }
    pausa = pnormal;
    for (int i = 0; i < tsp1 - (csp1 - csp0); i++) {
      AvanceBandeja(1);
      RetroTinta(1);
    }
  }
}

void ASBE(int steps) {
  pausa = p0;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p1;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p2;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p3;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p4;
  for (int i = 0; i < steps * 12 / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p3;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p2;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p1;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
  pausa = p0;
  for (int i = 0; i < steps / 20; i++) {
    AvanceBandeja(frecuencia);
    AvanceEnvase(1);
  }
}

void ASBT(int steps) {
  for (int i = 0; i < steps; i++) {
    AvanceBandeja(1);
    AvanceTinta(1);
  }
}

void ReiniciarBandeja() {
  sensor_bandeja = analogRead(A0);
  while (sensor_bandeja > 500) {
    sensor_bandeja = analogRead(A0);
    RetroBandeja(10);
  }
  while (sensor_bandeja < 100) {
    sensor_bandeja = analogRead(A0);
    AvanceBandeja(1);
  }
  PAB = 0;
}

void ReiniciarCarro() {
  sensor_carro = analogRead(A1);
  while (sensor_carro > 500) {
    sensor_carro = analogRead(A1);
    RetroCarro(10);
  }
  while (sensor_carro < 100) {
    sensor_carro = analogRead(A1);
    AvanceCarro(1);
  }
  PAC = 0;
}

void ReiniciarTinta() {
  sensor_tinta = analogRead(A2);
  while (sensor_tinta > 500) {
    sensor_tinta = analogRead(A2);
    RetroTinta(10);
  }
  while (sensor_tinta < 100) {
    sensor_tinta = analogRead(A2);
    AvanceTinta(1);
  }
  PAT = 0;
}

void ReinicioSincrono() {
  sensor_bandeja = analogRead(A0);
  sensor_carro = analogRead(A1);
  sensor_tinta = analogRead(A2);
  pausa = pausa / 3;
  while (sensor_bandeja > 500 && sensor_carro > 500 && sensor_tinta > 500) {
    for (int i = 0; i < 10; i++) {
      RetroBandeja(1);
      RetroCarro(1);
      RetroTinta(1);
    }
    sensor_bandeja = analogRead(A0);
    sensor_carro = analogRead(A1);
    sensor_tinta = analogRead(A2);
  }
  pausa = 3 * pausa / 2;
  while (sensor_carro > 500 && sensor_tinta > 500) {
    for (int i = 0; i < 10; i++) {
      RetroCarro(1);
      RetroTinta(1);
    }
    sensor_carro = analogRead(A1);
    sensor_tinta = analogRead(A2);
  }
  while (sensor_bandeja > 500 && sensor_tinta > 500) {
    for (int i = 0; i < 10; i++) {
      RetroBandeja(1);
      RetroTinta(1);
    }
    sensor_bandeja = analogRead(A0);
    sensor_tinta = analogRead(A2);
  }
  while (sensor_bandeja > 500 && sensor_carro > 500) {
    for (int i = 0; i < 10; i++) {
      RetroBandeja(1);
      RetroCarro(1);
    }
    sensor_bandeja = analogRead(A0);
    sensor_carro = analogRead(A1);
  }
  pausa = pnormal;
  ReiniciarBandeja();
  ReiniciarCarro();
  ReiniciarTinta();
}

void ReiniciarTodos() {
  pausa = prapida;
  ReiniciarBandeja();
  ReiniciarCarro();
  ReiniciarTinta();
  pausa = pnormal;
}

void RetrocederBandeja(double steps) {
  pausa = p0 * 120 / velocidad;
  for (int i = 0; i < steps / 20; i++) {
    RetroBandeja(1);
  }
  pausa = p1 * 120 / velocidad;
  for (int i = 0; i < steps / 20; i++) {
    RetroBandeja(1);
  }
  pausa = p2 * 120 / velocidad;
  for (int i = 0; i < (steps / 20); i++) {
    RetroBandeja(1);
  }
  pausa = p3 * 120 / velocidad;
  for (int i = 0; i < (steps / 20); i++) {
    RetroBandeja(1);
  }
  pausa = p4 * 120 / velocidad;
  for (int i = 0; i < (steps * 12 / 20); i++) {
    RetroBandeja(1);
  }
  pausa = p3 * 120 / velocidad;
  for (int i = 0; i < (steps / 20); i++) {
    RetroBandeja(1);
  }
  pausa = p2 * 120 / velocidad;
  for (int i = 0; i < (steps / 20); i++) {
    RetroBandeja(1);
  }
  pausa = p1 * 120 / velocidad;
  for (int i = 0; i < steps / 20; i++) {
    RetroBandeja(1);
  }
  pausa = p0 * 120 / velocidad;;
  for (int i = 0; i < steps / 20; i++) {
    RetroBandeja(1);
  }
}
void AvanceBandeja(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(BANDEJA1,  LOW);
    digitalWrite(BANDEJA2,  HIGH);
    digitalWrite(BANDEJA3,  LOW);
    digitalWrite(BANDEJA4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  HIGH);
    digitalWrite(BANDEJA2,  LOW);
    digitalWrite(BANDEJA3,  LOW);
    digitalWrite(BANDEJA4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  HIGH);
    digitalWrite(BANDEJA2,  LOW);
    digitalWrite(BANDEJA3,  HIGH);
    digitalWrite(BANDEJA4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  LOW);
    digitalWrite(BANDEJA2,  HIGH);
    digitalWrite(BANDEJA3,  HIGH);
    digitalWrite(BANDEJA4,  LOW);
    delayMicroseconds(pausa);
  }
  PAB = PAB + steps;
}

void RetroBandeja(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(BANDEJA1,  LOW);
    digitalWrite(BANDEJA2,  HIGH);
    digitalWrite(BANDEJA3,  LOW);;
    digitalWrite(BANDEJA4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  LOW);
    digitalWrite(BANDEJA2,  HIGH);
    digitalWrite(BANDEJA3,  HIGH);
    digitalWrite(BANDEJA4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  HIGH);
    digitalWrite(BANDEJA2,  LOW);
    digitalWrite(BANDEJA3,  HIGH);
    digitalWrite(BANDEJA4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(BANDEJA1,  HIGH);
    digitalWrite(BANDEJA2,  LOW);
    digitalWrite(BANDEJA3,  LOW);
    digitalWrite(BANDEJA4,  HIGH);
    delayMicroseconds(pausa);
  }
  PAB = PAB - steps;
}



void AvanceCarro(int steps) {
  steps = steps * 2;
  for (int i = 0; i < steps; i++) {
    digitalWrite(CARRO1,  LOW);
    digitalWrite(CARRO2,  HIGH);
    digitalWrite(CARRO3,  LOW);
    digitalWrite(CARRO4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  HIGH);
    digitalWrite(CARRO2,  LOW);
    digitalWrite(CARRO3,  LOW);
    digitalWrite(CARRO4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  HIGH);
    digitalWrite(CARRO2,  LOW);
    digitalWrite(CARRO3,  HIGH);
    digitalWrite(CARRO4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  LOW);
    digitalWrite(CARRO2,  HIGH);
    digitalWrite(CARRO3,  HIGH);
    digitalWrite(CARRO4,  LOW);
    delayMicroseconds(pausa);
  }
  PAC = PAC + steps;
}

void RetroCarro(int steps) {
  steps = steps * 2;
  for (int i = 0; i < steps; i++) {
    digitalWrite(CARRO1,  LOW);
    digitalWrite(CARRO2,  HIGH);
    digitalWrite(CARRO3,  LOW);;
    digitalWrite(CARRO4,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  LOW);
    digitalWrite(CARRO2,  HIGH);
    digitalWrite(CARRO3,  HIGH);
    digitalWrite(CARRO4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  HIGH);
    digitalWrite(CARRO2,  LOW);
    digitalWrite(CARRO3,  HIGH);
    digitalWrite(CARRO4,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(CARRO1,  HIGH);
    digitalWrite(CARRO2,  LOW);
    digitalWrite(CARRO3,  LOW);
    digitalWrite(CARRO4,  HIGH);
    delayMicroseconds(pausa);
  }
  PAC = PAC - steps;
}

void AvanceTinta(int steps) {
  steps = 2 * steps;
  for (int i = 0; i < steps; i++) {
    digitalWrite(38,  LOW);
    digitalWrite(40,  HIGH);
    digitalWrite(42,  LOW);
    digitalWrite(44,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(38,  HIGH);
    digitalWrite(40,  LOW);
    digitalWrite(42,  LOW);
    digitalWrite(44,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(38,  HIGH);
    digitalWrite(40,  LOW);
    digitalWrite(42,  HIGH);
    digitalWrite(44,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(38,  LOW);
    digitalWrite(40,  HIGH);
    digitalWrite(42,  HIGH);
    digitalWrite(44,  LOW);
    delayMicroseconds(pausa);
  }
  PAT = PAT + steps;
}

void RetroTinta(int steps) {
  steps = 2 * steps;
  for (int i = 0; i < steps; i++) {
    digitalWrite(38,  LOW);
    digitalWrite(40,  HIGH);
    digitalWrite(42,  LOW);;
    digitalWrite(44,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(38,  LOW);
    digitalWrite(40,  HIGH);
    digitalWrite(42,  HIGH);
    digitalWrite(44,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(38,  HIGH);
    digitalWrite(40,  LOW);
    digitalWrite(42,  HIGH);
    digitalWrite(44,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(38,  HIGH);
    digitalWrite(40,  LOW);
    digitalWrite(42,  LOW);
    digitalWrite(44,  HIGH);
    delayMicroseconds(pausa);
  }
  PAT = PAT - steps;
}

void AvanceEnvase(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(46,  LOW);
    digitalWrite(48,  HIGH);
    digitalWrite(50,  LOW);
    digitalWrite(52,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(46,  HIGH);
    digitalWrite(48,  LOW);
    digitalWrite(50,  LOW);
    digitalWrite(52,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(46,  HIGH);
    digitalWrite(48,  LOW);
    digitalWrite(50,  HIGH);
    digitalWrite(52,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(46,  LOW);
    digitalWrite(48,  HIGH);
    digitalWrite(50,  HIGH);
    digitalWrite(52,  LOW);
    delayMicroseconds(pausa);
  }
  PAE = PAE + steps;
}

void RetroEnvase(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(46,  LOW);
    digitalWrite(48,  HIGH);
    digitalWrite(50,  LOW);;
    digitalWrite(52,  HIGH);
    delayMicroseconds(pausa);
    digitalWrite(46,  LOW);
    digitalWrite(48,  HIGH);
    digitalWrite(50,  HIGH);
    digitalWrite(52,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(46,  HIGH);
    digitalWrite(48,  LOW);
    digitalWrite(50,  HIGH);
    digitalWrite(52,  LOW);
    delayMicroseconds(pausa);
    digitalWrite(46,  HIGH);
    digitalWrite(48,  LOW);
    digitalWrite(50,  LOW);
    digitalWrite(52,  HIGH);
    delayMicroseconds(pausa);
  }
  PAE = PAE - steps;
}
