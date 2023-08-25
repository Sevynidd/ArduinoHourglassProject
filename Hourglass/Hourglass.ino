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
  matrix.control(MD_MAX72XX::INTENSITY, 1);
  matrix.clear();

  //Gyro
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(100);
  mpu.calcGyroOffsets();
  Serial.println("Done.");

  for (int i = 0; i <= 7; i++) {
    matrix.setColumn(i, 0xff);
  }
}

void loop() {
  mpu.update();

  Serial.print("X: " + String(mpu.getAngleX()));
  Serial.println("\tY: " + String(mpu.getAngleY()));

  delay(50);
  //0xff = HIGH; 0x00 = LOW
  //matrix.setRow(0, 0xff);
  //matrix.setPoint(3, 0, true);
  //matrix.setPoint(7, 9, true);
}
