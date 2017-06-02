/*
  Пример работы с внешним датчиком температуры/влажности AM2320
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <AM2320.h>

AM2320 sensor_am2320;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  sensor_am2320.begin();
  delay(1000);
}

void loop() {
  // Измерение
  sensor_am2320.measure();
  float t = sensor_am2320.getTemperature();
  float h = sensor_am2320.getHumidity();
  // Вывод измеренных значений в терминал
  Serial.println("Temperature: " + String(t, 1) + " *C");
  Serial.println("Humidity: " + String(h, 1) + " %");
  Serial.println();
  delay(1000);
}

