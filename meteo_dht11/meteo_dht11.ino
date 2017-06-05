/*
  Пример работы с внешним датчиком температуры/влажности DHT11
  Created by Rostislav Varzar, MGBot
*/

#include <DHT.h>

#define DHTPIN 14
DHT dht(DHTPIN, DHT11, 15);

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  dht.begin();
  delay(1000);
}

void loop() {
  // Измерение
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Вывод измеренных значений в терминал
  Serial.println("Temperature: " + String(t, 1) + " *C");
  Serial.println("Humidity: " + String(h, 1) + " %");
  Serial.println();
  delay(1000);
}

