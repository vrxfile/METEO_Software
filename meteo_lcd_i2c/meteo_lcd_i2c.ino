/*
  Пример работы с I2C LCD дисплеем 16x2
  Created by Rostislav Varzar, MGBot
*/

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(1000);

  // Инициализация дисплея
  lcd.init();       // Инициализация
  lcd.backlight();  // Включение подсветки
  lcd.clear();      // Очистка экрана
  delay(1000);

  // Вывод текста
  lcd.setCursor(0, 0);            // Устанавливаем позицию курсора
  lcd_printstr("Hello world!");   // Выводим текст
  lcd.setCursor(0, 1);            // Устанавливаем позицию курсора
  lcd_printstr("Hello world!");   // Выводим текст
}

void loop() {
}

// Функция вывода текстовой строки на дисплей
void lcd_printstr(String str1) {
  for (int i = 0; i < str1.length(); i++) {
    lcd.print(str1.charAt(i));
  }
}

