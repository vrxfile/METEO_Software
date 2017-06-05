/*
  Пример работы с интегрированным АЦП ADS1115
  Created by Rostislav Varzar, MGBot
*/

#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация АЦП
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
  delay(1000);
}

void loop() {
  // Измерение
  float adc0 = (float)ads.readADC_SingleEnded(0) / 32767 * 6.144;
  float adc1 = (float)ads.readADC_SingleEnded(1) / 32767 * 6.144;
  float adc2 = (float)ads.readADC_SingleEnded(2) / 32767 * 6.144;
  float adc3 = (float)ads.readADC_SingleEnded(3) / 32767 * 6.144;
  // Вывод измеренных значений в терминал
  Serial.println("ADC0: " + String(adc0, 3) + " V");
  Serial.println("ADC1: " + String(adc1, 3) + " V");
  Serial.println("ADC2: " + String(adc2, 3) + " V");
  Serial.println("ADC3: " + String(adc3, 3) + " V");
  Serial.println();
  delay(1000);
}

