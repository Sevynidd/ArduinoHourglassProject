//Matrix
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 2
#define CS_PIN 10
#define DATA_PIN 11
#define CLK_PIN 13

//Gyro
#include <Wire.h>
#include <MPU6050_light.h>

MD_Parola matrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

MPU6050 mpu(Wire);

void setup()
{
  Serial.begin(9600);
  Serial.flush();

  //LED
  matrix.begin();
  matrix.setIntensity(0);
  matrix.displayClear();
    
  //Gyro
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(100);
  mpu.calcGyroOffsets();
  Serial.println("Done.");
}

void loop()
{
    mpu.update();

    Serial.print("X: " + String(mpu.getAngleX()));
    Serial.println("\tY: " + String(mpu.getAngleY()));

    delay(50);

    matrix.setTextAlignment(PA_CENTER);
    matrix.print("CT");
  delay(500);
}
