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
  mpu.update();
  if(mpu.getAngleY()<0){
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

  Serial.print("X: " + String(mpu.getAngleX()));
  Serial.println("\tY: " + String(mpu.getAngleY()));
    
  //Wert größer als die Möglichkeiten setzen


  if (state == 0) {
    delay(100);
  } else if (state == 60) {
    delay(100);
  }

  if ((state < 60) & kopf) {

    //Werte die auf jeden Fall überschrieben werden setzen, um vorbelegung zu entfernen
    setzenX = 8;
    setzenY = 8;
    entfernenX = 8;
    entfernenY = 8;

    //die folgenden Koordinaten sind immer + 1, da sie für das Füllen der Matrix dann besser geeignet sind
    //i-1 und j-1 sind die Koordinaten die entfernt werden
    for (int i=1; i<=8; i++) {
      for (int j=9; j<=16; j++) {
        if (matrix.getPoint(i-1, j-1)) {
          if ((i*(j-8)) < (entfernenX*entfernenY)) {
            entfernenX = i;
            entfernenY = j-8;
          }
        }
      }
    }

    matrix.setPoint(entfernenX-1, entfernenY+7, false);

//i-1 und j-1 sind die koordinaten, die hinzugefügt werden
    for (int i=1; i<=8; i++) {
      for (int j=1; j<=8; j++) {
        if (matrix.getPoint(i-1, j-1) == false) {
          if ((i*j) < (setzenX*setzenY)) {
            setzenX = i;
            setzenY = j;
          }
        }
      }
    }
	
    matrix.setPoint(setzenX-1, setzenY-1, true);
    state++;
	
  } else if ((state > 0) & !kopf) {
    setzenX = 8;
    setzenY = 8;

    entfernenX = 8;
    entfernenY = 8;


    //Niedrigsten freien Punkt suchen
    for (int i=1; i<=8; i++) {
      for (int j=1; j<=8; j++) {
        if (matrix.getPoint(i-1, j-1)) {
          if ((i*j) < (entfernenX*entfernenY)) {
            entfernenX = i;
            entfernenY = j;
          }
        }
      }
    } 
    //i-1 und j-1 sind die koordinaten, die hinzugefügt werden
    matrix.setPoint(entfernenX-1, entfernenY-1, false);

    for (int i=1; i<=8; i++) {
      for (int j=9; j<=16; j++) {
        if (matrix.getPoint(i-1, j-1) == false) {
          if ((i*(j-8)) < (setzenX*setzenY)) {
            setzenX = i;
            setzenY = j-8;
          }
        }
      }
    }  
  
    matrix.setPoint(setzenX-1, setzenY+7, true);
    Serial.println(setzenX);
    Serial.println(setzenY);
    state--;
  }


  delay(100);
  //0xff = HIGH; 0x00 = LOW
  //matrix.setRow(0, 0xff);
  //matrix.setPoint(3, 0, true);
  //matrix.setPoint(7, 9, true);

}


void swap() {
  
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      if (matrix.getPoint(i,j)) {
        hilfsmatrix[7-i][7-j] = true;
      }
      else {
        hilfsmatrix[7-i][7-j] = false;
      }
    }
  }
  
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      matrix.setPoint(i, j, hilfsmatrix[i][j]);
    }
  }

  for (int i=0; i<8; i++) {
    for (int j=8; j<16; j++) {
      if (matrix.getPoint(i,j)) {
        hilfsmatrix[7-i][7-(j-8)] = true;
      }
      else {
        hilfsmatrix[7-i][7-(j-8)] = false;
      }
    }
  }
  
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      matrix.setPoint(i, j+8, hilfsmatrix[i][j]);
    }
  }
}
