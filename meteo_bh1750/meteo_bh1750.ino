/*
  Пример работы с интегрированным датчиком освещенности BH1750FVI
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <BH1750FVI.h>

BH1750FVI LightSensor;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  LightSensor.begin();
  LightSensor.setMode(Continuously_High_Resolution_Mode);
  delay(1000);
}

void loop() {
  // Измерение
  float lux = LightSensor.getAmbientLight();

  // Вывод измеренных значений в терминал
  Serial.println("Ambient light: " + String(lux, 1) + " lux");
  Serial.println();
  delay(1000);
}

