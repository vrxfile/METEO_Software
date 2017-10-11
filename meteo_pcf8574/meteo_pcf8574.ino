/*
  Пример работы со интегрированным на плату расширителем портов PCF8574 (тест управления реле)
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <pcf8574_esp.h>

//TwoWire testWire;
//PCF857x pcf8574(0x20, &testWire);
PCF857x pcf8574(0x20, &Wire);

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация PCF8574
  Wire.begin(4, 5);
  Wire.setClock(100000L);
  pcf8574.begin();

  // Инициализация выходов
  pcf8574.write(0, LOW);
  pcf8574.write(1, LOW);
  pcf8574.write(2, LOW);
  pcf8574.write(3, LOW);
}

void loop() {
  Serial.println("Relay #1 ON");
  pcf8574.write(0, HIGH);
  delay(1000);
  Serial.println("Relay #2 ON");
  pcf8574.write(1, HIGH);
  delay(1000);
  Serial.println("Relay #3 ON");
  pcf8574.write(2, HIGH);
  delay(1000);
  Serial.println("Relay #4 ON");
  pcf8574.write(3, HIGH);
  delay(5000);
  Serial.println("Relay #1 OFF");
  pcf8574.write(0, LOW);
  delay(1000);
  Serial.println("Relay #2 OFF");
  pcf8574.write(1, LOW);
  delay(1000);
  Serial.println("Relay #3 OFF");
  pcf8574.write(2, LOW);
  delay(1000);
  Serial.println("Relay #4 OFF");
  pcf8574.write(3, LOW);
  delay(5000);
}

