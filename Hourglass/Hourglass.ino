//Matrix
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 2
#define CS_PIN 10
#define DATA_PIN 11
#define CLK_PIN 13

int mapMatrix1[8][8];
int mapMatrix2[8][8];
bool hilfsmatrix[8][8];

bool kopf = false; //Wird anhand der Ausrichtung des Gyros bestimmt
int setzenX = 0; //X Koordinate die beschrieben werden soll
int setzenY = 0; //Y Koordinate die beschrieben werden soll
int entfernenX = 0; //X Koordinate die entfernt werden soll
int entfernenY = 0; //Y Koordinate die entfernt werden soll
int state = 0; //Intervall für Sekunden pro Minute

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

//Gyro
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

void setup() {
  Serial.begin(9600);
  Serial.flush();

  //LED
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 0.2);
  matrix.clear();
  
  // Setzen der unteren 60 LEDs (weil 60sek/min)
  for (int i = 8; i <= 15; i++) {    
    matrix.setColumn(i, 0xff);
  }
  matrix.setPoint(7, 15, false);
  matrix.setPoint(6, 15, false);
  matrix.setPoint(7, 14, false);
  matrix.setPoint(6, 14, false);

  //Gyro
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(100);
  mpu.calcGyroOffsets();
  Serial.println("Done.");
  
}
     
void loop() {
  //update des Gyros
  mpu.update();

  //Ausgabe der Gyro-Werte auf dem Serial Monitor
    Serial.print("X: " + String(mpu.getAngleX()));
    Serial.println("\tY: " + String(mpu.getAngleY()));
    

  //Überprüfung der Richtung der Sanduhr
  if(mpu.getAngleZ()<0){
    if(kopf==true){
      swap();
    }
    kopf = false;
  }
  else{
    if(kopf==false){
      swap();
    }
    kopf = true;
  }

    //Werte die auf jeden Fall überschrieben werden setzen, um vorbelegung zu entfernen
    setzenX = 8;
    setzenY = 8;
    entfernenX = 8;
    entfernenY = 8;

//Teil der ausgeführt wird, wenn die Sanduhr auf dem Kopf steht und die maximalen Veränderungen noch nicht eingetreten sind
  if ((state < 60) & kopf) {

    //i und j sind die Koordinaten die überprüft werden
    for (int i=0; i<8; i++) {
      for (int j=8; j<16; j++) {
        //suchen der leuchtenden LED mit dem geringsten Wert basierend auf der Multiplikation
        if (matrix.getPoint(i, j)) {
          if (((i+1)*(j-7)) < ((entfernenX+1)*(entfernenY+1))) {
            entfernenX = i;
            entfernenY = j-8;
          }
        }
      }
    }

    matrix.setPoint(entfernenX, entfernenY+8, false);

//i und j sind die koordinaten, die überprüft werden
    for (int i=0; i<8; i++) {
      for (int j=0; j<8; j++) {
        //suchen der nicht leuchtenden LED mit dem geringsten Wert basierend auf der Multiplikation
        if (matrix.getPoint(i, j) == false) {
          if (((i+1)*(j+1)) < ((setzenX+1)*(setzenY+1))) {
            setzenX = i;
            setzenY = j;
          }
        }
      }
    }
	
    matrix.setPoint(setzenX, setzenY, true);

    //folgen des Zustands, damit die Schleifen nur begrenzt laufen
    state++;
	
  }
  
//Teil der ausgeführt wird, wenn die Sanduhr nicht auf dem Kopf steht und die maximalen Veränderungen noch nicht eingetreten sind
  else if ((state > 0) & !kopf) {
//i und j sind die koordinaten, die überprüft werden
    for (int i=0; i<8; i++) {
      for (int j=0; j<8; j++) {
        //suchen der leuchtenden LED mit dem geringsten Wert basierend auf der Multiplikation
        if (matrix.getPoint(i, j)) {
          if (((i+1)*(j+1) < ((entfernenX+1)*(entfernenY+1)))) {
            entfernenX = i;
            entfernenY = j;
          }
        }
      }
    } 
    matrix.setPoint(entfernenX, entfernenY, false);

//i und j sind die koordinaten, die überprüft werden
    for (int i=0; i<8; i++) {
      for (int j=8; j<16; j++) {
       //suchen der nicht leuchtenden LED mit dem geringsten Wert basierend auf der Multiplikation
        if (matrix.getPoint(i, j) == false) {
          if (((i+1)*(j-7)) < ((setzenX+1)*(setzenY+1))) {
            setzenX = i;
            setzenY = j-8;
          }
        }
      }
    }  
  
    matrix.setPoint(setzenX, setzenY+8, true);

    //folgen des Zustands, damit die Schleifen nur begrenzt laufen
    state--;
  }

  //Zeitmesser
  delay(1000);

}

//Funktion für das Fallen der Sandkörner in die Entgegengesetzte Richtung
void swap() {
  //durchiterieren durch die 1. Matrix
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      //lesen eines Wertes der Matrix
      if (matrix.getPoint(i,j)) {
        //setzen des true-Wertes auf dem entgegengesetzten Feld in einer Hilfsmatrix
        hilfsmatrix[7-i][7-j] = true;
      }
      else {
        //setzen des false-Wertes auf dem entgegengesetzten Feld in einer Hilfsmatrix
        hilfsmatrix[7-i][7-j] = false;
      }
    }
  }
  
  //schreiben der Hilfsmatrix in die originale 1.Matrix
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      matrix.setPoint(i, j, hilfsmatrix[i][j]);
    }
  }

  //durchiterieren durch die 2. Matrix
  for (int i=0; i<8; i++) {
    for (int j=8; j<16; j++) {
       //lesen eines Wertes der Matrix
      if (matrix.getPoint(i,j)) {
        //setzen des true-Wertes auf dem entgegengesetzten Feld in einer Hilfsmatrix
        hilfsmatrix[7-i][7-(j-8)] = true;
      }
      else {
        //setzen des false-Wertes auf dem entgegengesetzten Feld in einer Hilfsmatrix
        hilfsmatrix[7-i][7-(j-8)] = false;
      }
    }
  }
  
  //schreiben der Hilfsmatrix in die originale 2.Matrix

  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      matrix.setPoint(i, j+8, hilfsmatrix[i][j]);
    }
  }
}
