#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <pcf8574_esp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_LSM303_U.h>
#include <BH1750FVI.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>

// Точка доступа Wi-Fi
char ssid[] = "MGBot";
char pass[] = "Terminator812";

// Расширитель портов
PCF857x pcf8574(0x20, &Wire);

// Датчик освещенности
BH1750FVI bh1750;

// Датчик температуры/влажности и атмосферного давления
Adafruit_BME280 bme280;

// Датчик ускорения/магнитного поля
Adafruit_LSM303_Mag_Unified mag303 = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel303 = Adafruit_LSM303_Accel_Unified(54321);

// Аналогово-цифровой преобразователь
Adafruit_ADS1115 ads1115;

// Датчик влажности и температуры почвы емкостной
const float air_value1 = 83900.0;
const float water_value1 = 45000.0;
const float moisture_0 = 0.0;
const float moisture_100 = 100.0;

// Периоды для таймеров
#define BME280_UPDATE_TIME  5000
#define BH1750_UPDATE_TIME  5000
#define LSM303_UPDATE_TIME  5000
#define ADS1115_UPDATE_TIME 5000
#define IOT_UPDATE_TIME     10000

// Таймеры
SimpleTimer timer_bme280;
SimpleTimer timer_bh1750;
SimpleTimer timer_lsm303;
SimpleTimer timer_ads1115;
SimpleTimer timer_iot;

// Состояния управляющих устройств
int light_control = 0;
int pump_control = 0;
int air_control = 0;

// Параметры IoT сервера
char iot_server[] = "academic-educatorsextension.portal.ptc.io";
IPAddress iot_address(34, 224, 244, 58);
char thingName[] = "meteoboard_thing_rostislav";
char serviceName[] = "meteo_board_service";
char appKey[] = "44cae228-fc33-4c83-9a44-b422dc7b60d3";

// Параметры сенсоров для IoT сервера
#define sensorCount 12
char* sensorNames[] = {"air_temp", "air_hum", "air_press", "sun_light", "soil_temp", "soil_hum",
                       "acc_x", "acc_y", "acc_z", "mag_x", "mag_y", "mag_z"
                      };
float sensorValues[sensorCount];
// Номера датчиков
#define air_temp     0x00
#define air_hum      0x01
#define air_press    0x02
#define sun_light    0x03
#define soil_temp    0x04
#define soil_hum     0x05
#define acc_x        0x06
#define acc_y        0x07
#define acc_z        0x08
#define mag_x        0x09
#define mag_y        0x0A
#define mag_z        0x0B

// Максимальное время ожидания ответа от сервера
#define IOT_TIMEOUT1 5000
#define IOT_TIMEOUT2 100

// Таймер ожидания прихода символов с сервера
long timer_iot_timeout = 0;

// Размер приемного буффера
#define BUFF_LENGTH 256

// Приемный буфер
char buff[BUFF_LENGTH] = "";

void setup()
{
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(512);

  // Инициализация Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Инициализация интерфейса I2C
  Wire.begin(4, 5);
  Wire.setClock(100000L);

  // Инициализация расширителя портов
  pcf8574.begin();
  pcf8574.write(0, LOW);
  pcf8574.write(1, LOW);
  pcf8574.write(2, LOW);
  pcf8574.write(3, LOW);
  pcf8574.write(4, LOW);
  pcf8574.write(5, LOW);
  pcf8574.write(6, LOW);
  pcf8574.write(7, LOW);

  //Инициализация АЦП
  ads1115.setGain(GAIN_TWOTHIRDS);
  ads1115.begin();

  // Инициализация датчика BH1750
  bh1750.begin();
  bh1750.setMode(Continuously_High_Resolution_Mode);

  // Инициализация датчика BME280
  bme280.begin();

  // Инициализация датчика LSM303
  mag303.enableAutoRange(true);
  mag303.begin();
  accel303.begin();

  // Однократный опрос датчиков
  readSensorBME280();
  readSensorBH1750();
  readSensorLSM303();
  readSensorADS1115();

  // Вывод в терминал данных с датчиков
  printAllSensors();

  // Инициализация таймеров
  timer_bme280.setInterval(BME280_UPDATE_TIME, readSensorBME280);
  timer_bh1750.setInterval(BH1750_UPDATE_TIME, readSensorBH1750);
  timer_lsm303.setInterval(LSM303_UPDATE_TIME, readSensorLSM303);
  timer_ads1115.setInterval(ADS1115_UPDATE_TIME, readSensorADS1115);
  timer_iot.setInterval(IOT_UPDATE_TIME, sendDataIot);
}

void loop()
{
  timer_bme280.run();
  timer_bh1750.run();
  timer_lsm303.run();
  timer_ads1115.run();
  timer_iot.run();
}

// Чтение датчика BME280
void readSensorBME280()
{
  sensorValues[air_temp] = bme280.readTemperature();
  sensorValues[air_hum] = bme280.readHumidity();
  sensorValues[air_press] = bme280.readPressure() * 7.5006 / 1000.0;
}

// Чтение датчика BH1750
void readSensorBH1750()
{
  sensorValues[sun_light] = bh1750.getAmbientLight();
}

// Чтение датчика LSM303
void readSensorLSM303()
{
  sensors_event_t acc_event;
  accel303.getEvent(&acc_event);
  sensors_event_t mag_event;
  mag303.getEvent(&mag_event);
  sensorValues[acc_x] = acc_event.acceleration.x;
  sensorValues[acc_y] = acc_event.acceleration.y;
  sensorValues[acc_z] = acc_event.acceleration.z;
  sensorValues[mag_x] = mag_event.magnetic.x;
  sensorValues[mag_y] = mag_event.magnetic.y;
  sensorValues[mag_z] = mag_event.magnetic.z;
}

// Чтение аналоговых датчиков через АЦП
void readSensorADS1115()
{
  float adc0 = (float)ads1115.readADC_SingleEnded(0) * 6.144;
  float adc1 = (float)ads1115.readADC_SingleEnded(1) * 6.144;
  //Serial.println("ADC0: " + String(adc0, 3));
  //Serial.println("ADC1: " + String(adc1, 3));
  sensorValues[soil_hum] = map(adc0, air_value1, water_value1, moisture_0, moisture_100);
  sensorValues[soil_temp] = adc1 / 1000.0;
}

// Print sensors data to terminal
void printAllSensors()
{
  for (int i = 0; i < sensorCount; i++)
  {
    Serial.print(sensorNames[i]);
    Serial.print(" = ");
    Serial.println(sensorValues[i]);
  }
  Serial.println();
}

// Подключение к серверу IoT ThingWorx
void sendDataIot()
{
  // Подключение к серверу
  WiFiClientSecure client;
  Serial.println("Connecting to IoT server...");
  if (client.connect(iot_address, 443))
  {
    // Проверка установления соединения
    if (client.connected())
    {
      // Отправка заголовка сетевого пакета
      Serial.println("Sending data to IoT server...\n");
      Serial.print("POST /Thingworx/Things/");
      client.print("POST /Thingworx/Things/");
      Serial.print(thingName);
      client.print(thingName);
      Serial.print("/Services/");
      client.print("/Services/");
      Serial.print(serviceName);
      client.print(serviceName);
      Serial.print("?appKey=");
      client.print("?appKey=");
      Serial.print(appKey);
      client.print(appKey);
      Serial.print("&method=post&x-thingworx-session=true");
      client.print("&method=post&x-thingworx-session=true");
      // Отправка данных с датчиков
      for (int idx = 0; idx < sensorCount; idx ++)
      {
        Serial.print("&");
        client.print("&");
        Serial.print(sensorNames[idx]);
        client.print(sensorNames[idx]);
        Serial.print("=");
        client.print("=");
        Serial.print(sensorValues[idx]);
        client.print(sensorValues[idx]);
      }
      // Закрываем пакет
      Serial.println(" HTTP/1.1");
      client.println(" HTTP/1.1");
      Serial.println("Accept: application/json");
      client.println("Accept: application/json");
      Serial.print("Host: ");
      client.print("Host: ");
      Serial.println(iot_server);
      client.println(iot_server);
      Serial.println("Content-Type: application/json");
      client.println("Content-Type: application/json");
      Serial.println();
      client.println();

      // Ждем ответа от сервера
      timer_iot_timeout = millis();
      while ((client.available() == 0) && (millis() < timer_iot_timeout + IOT_TIMEOUT1));

      // Выводим ответ о сервера, и, если медленное соединение, ждем выход по таймауту
      int iii = 0;
      bool currentLineIsBlank = true;
      bool flagJSON = false;
      timer_iot_timeout = millis();
      while ((millis() < timer_iot_timeout + IOT_TIMEOUT2) && (client.connected()))
      {
        while (client.available() > 0)
        {
          char symb = client.read();
          Serial.print(symb);
          if (symb == '{')
          {
            flagJSON = true;
          }
          else if (symb == '}')
          {
            flagJSON = false;
          }
          if (flagJSON == true)
          {
            buff[iii] = symb;
            iii ++;
          }
          timer_iot_timeout = millis();
        }
      }
      buff[iii] = '}';
      buff[iii + 1] = '\0';
      Serial.println(buff);
      // Закрываем соединение
      client.stop();

      // Расшифровываем параметры
      StaticJsonBuffer<BUFF_LENGTH> jsonBuffer;
      JsonObject& json_array = jsonBuffer.parseObject(buff);
      if (json_array.success())
      {
        pump_control = json_array["pump_control"];
        light_control = json_array["light_control"];
        air_control = json_array["air_control"];
        Serial.println("Pump state:   " + String(pump_control));
        Serial.println("Light state:  " + String(light_control));
        Serial.println("Air state:    " + String(air_control));
        Serial.println();
      }
      else
      {
        Serial.println("Fail to parse packet from Thingworx!");
        Serial.println();
      }

      // Делаем управление устройствами
      controlDevices();

      Serial.println("Packet successfully sent!");
      Serial.println();
    }
  }
}

// Управление исполнительными устройствами
void controlDevices()
{
  // Помпа
  pcf8574.write(0, pump_control);
  // Освещение
  pcf8574.write(1, light_control);
  // Вентиляция
  pcf8574.write(2, air_control);
}

