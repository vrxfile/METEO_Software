/*
  Пример работы с внешним датчиком температуры DS18B20
  Created by Rostislav Varzar, MGBot
*/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация датчика
  ds18b20.begin();
  delay(1000);
}

void loop() {
  // Измерение
  ds18b20.requestTemperatures();
  float t = ds18b20.getTempCByIndex(0);
  // Вывод измеренных значений в терминал
  Serial.println("Temperature: " + String(t, 1) + " *C");
  Serial.println();
  delay(1000);
}

