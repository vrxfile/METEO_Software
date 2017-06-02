/*
  Пример работы с интегрированным датчиком температуры/влажности/давления BME280
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  delay(1000);
}

void loop() {
  // Измерение
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0F;
  float a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  // Вывод измеренных значений в терминал
  Serial.println("Temperature: " + String(t, 1) + " *C");
  Serial.println("Humidity: " + String(h, 1) + " %");
  Serial.println("Pressure: " + String(p, 1) + " hPa");
  Serial.println("Altitude: " + String(a, 1) + " m");
  Serial.println();
  delay(1000);
}

