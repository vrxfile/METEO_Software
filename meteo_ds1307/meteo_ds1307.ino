/*
  Пример работы со интегрированным на плату RTC (часы реального времени) на микросхеме DS1307
  Created by Rostislav Varzar, MGBot
*/

#include <DS1307.h>

DS1307 ds_clock;
RTCDateTime dt;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация микросхемы
  ds_clock.begin();

  // Если микросхема используется в первый раз - установка текущего времени (времени компиляции программы)
  if (!ds_clock.isReady())
  {
    ds_clock.setDateTime(__DATE__, __TIME__);
  }
}

void loop() {
  // Чтение времени из микросхемы
  dt = ds_clock.getDateTime();
  // Вывод считанного времени и даты в терминал
  Serial.print("Date and time: ");
  Serial.print(dt.year);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.day);    Serial.print(" ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println();
  delay(1000);
}

