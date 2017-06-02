/*
  Пример работы с интегрированным акселерометром/компасом LSM303DLHC
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  mag.enableAutoRange(true);
  bool status = mag.begin();
  if (!status) {
    Serial.println("Could not find a valid LSM303DLHC sensor, check wiring!");
    while (1);
  }
  delay(1000);
  status = accel.begin();
  if (!status) {
    Serial.println("Could not find a valid LSM303DLHC sensor, check wiring!");
    while (1);
  }
  delay(1000);
}

void loop() {
  // Измерение
  sensors_event_t acc_event;
  accel.getEvent(&acc_event);
  sensors_event_t mag_event;
  mag.getEvent(&mag_event);
  float acc_x = acc_event.acceleration.x;
  float acc_y = acc_event.acceleration.y;
  float acc_z = acc_event.acceleration.z;
  float mag_x = mag_event.magnetic.x;
  float mag_y = mag_event.magnetic.y;
  float mag_z = mag_event.magnetic.z;
  // Вывод измеренных значений в терминал
  Serial.println("ACC X: " + String(acc_x, 1) + " m/s^2");
  Serial.println("ACC Y: " + String(acc_y, 1) + " m/s^2");
  Serial.println("ACC Z: " + String(acc_z, 1) + " m/s^2");
  Serial.println("MAG X: " + String(mag_x, 1) + " uT");
  Serial.println("MAG Y: " + String(mag_y, 1) + " uT");
  Serial.println("MAG Z: " + String(mag_z, 1) + " uT");
  Serial.println();
  delay(1000);
}

