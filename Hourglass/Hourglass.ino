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

//Gyro
#include <Wire.h>
#include <MPU6050_light.h>

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

MPU6050 mpu(Wire);

void setMapMatrix(int matrix){
  
}

void getRowAndColumnFromID(int ID, int matrix) {
  
}



void setup() {
  Serial.begin(9600);
  Serial.flush();

  //LED
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 0.2);
  matrix.clear();

  //Gyro
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(100);
  mpu.calcGyroOffsets();
  Serial.println("Done.");


  // setzen der unteren 60 LEDs (weil 60sek/min)
  for (int i = 8; i <= 15; i++) {    
    matrix.setColumn(i, 0xff);
  }
  matrix.setPoint(7, 8, false);
  matrix.setPoint(6, 8, false);
  matrix.setPoint(7, 9, false);
  matrix.setPoint(6, 9, false);
}
void swapUpwards(){
    for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        if(matrix.getPoint(i,j)){

            matrix.setPoint(7-i,7-j, true);
            matrix.setPoint(i,j, false);
          
        }
      }
    }
}

void swapDownwards(){
    for(int i=7; i>=0; i--){
      for(int j=7; j>=0; j--){
        if(matrix.getPoint(i,j)){
            matrix.setPoint(7-i,7-j, true);
            matrix.setPoint(i,j, false);

          }
        }
      }
    }
  


  bool kopf = true;
  int currX = 0; //X Koordinate die beschrieben werden soll
  int currY = 0; //Y Koordinate die beschrieben werden soll
  int state = 0;
void loop() {
  mpu.update();
    // swapDownwards();
    // delay(1000);

    // swapUpwards();
    // delay(1000);
  if (state == 60 & kopf){

  }
  else{
  Serial.print("X: " + String(mpu.getAngleX()));
  Serial.println("\tY: " + String(mpu.getAngleY()));
  currX = 8;
  currY = 8;
  for(int i=1; i<=8; i++){
    for(int j=1; j<=8; j++){
      if(matrix.getPoint(i-1, j-1)==false){
        if(i*j < currX*currY){
          currX = i;
          currY = j;
        }
      }
    }
  }
  matrix.setPoint(currX-1, currY-1, true);
  Serial.println(currX);
  Serial.println(currY);
  state++;



  delay(100);
  //0xff = HIGH; 0x00 = LOW
  //matrix.setRow(0, 0xff);
  //matrix.setPoint(3, 0, true);
  //matrix.setPoint(7, 9, true);
  }
}


