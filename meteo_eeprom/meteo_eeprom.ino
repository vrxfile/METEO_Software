/*
  Пример работы со интегрированной на плату памятью EEPROM 24LC256
  Created by Rostislav Varzar, MGBot
*/

#include <i2c_eeprom.h>

i2c_eeprom eeprom(0x50);

int counter_power = 0;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // Чтение первого байта памяти
  counter_power = eeprom.read(0);
  // Увеличение на 1 считанного значения
  counter_power = counter_power + 1;
  // Запись нового значения в первый байт памяти
  eeprom.write(0, (counter_power) & 0xFF);
  // Вывод нового значения "количества включений платы"
  Serial.println("Power on counts: " + String(counter_power));
  delay(1000);
}

