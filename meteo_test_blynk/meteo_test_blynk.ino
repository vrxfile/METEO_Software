#define BLYNK_PRINT Serial

#include <Wire.h>
#include <pcf8574_esp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_LSM303_U.h>
#include <BH1750FVI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Точка доступа Wi-Fi
char ssid[] = "IOTIK";
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

// API key для Blynk
char auth[] = "a45d999b33c940d6a584249193ef0f5d";
IPAddress blynk_ip(139, 59, 206, 133);

// Периоды для таймеров
#define BME280_UPDATE_TIME  5100
#define BH1750_UPDATE_TIME  5200
#define LSM303_UPDATE_TIME  5300
#define ADS1115_UPDATE_TIME 5400

// Таймеры
BlynkTimer timer1;
BlynkTimer timer2;
BlynkTimer timer3;
BlynkTimer timer4;

// Состояния управляющих устройств
int light_control = 0;
int pump_control = 0;
int air_control = 0;

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

void setup()
{
  // Инициализация последовательного порта
  Serial.begin(115200);
  delay(512);

  // Инициализация Blynk и Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(auth, ssid, pass, blynk_ip, 8442);

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
  timer1.setInterval(BME280_UPDATE_TIME, readSensorBME280);
  timer2.setInterval(BH1750_UPDATE_TIME, readSensorBH1750);
  timer3.setInterval(LSM303_UPDATE_TIME, readSensorLSM303);
  timer4.setInterval(ADS1115_UPDATE_TIME, readSensorADS1115);
}

void loop()
{
  Blynk.run();
  timer1.run();
  timer2.run();
  timer3.run();
  timer4.run();
}

// Чтение датчика BME280
void readSensorBME280()
{
  sensorValues[air_temp] = bme280.readTemperature();
  sensorValues[air_hum] = bme280.readHumidity();
  sensorValues[air_press] = bme280.readPressure() * 7.5006 / 1000.0;
  Blynk.virtualWrite(V0, sensorValues[air_temp]); delay(25);
  Blynk.virtualWrite(V1, sensorValues[air_hum]); delay(25);
  Blynk.virtualWrite(V2, sensorValues[air_press]); delay(25);
}

// Чтение датчика BH1750
void readSensorBH1750()
{
  sensorValues[sun_light] = bh1750.getAmbientLight();
  Blynk.virtualWrite(V3, sensorValues[sun_light]); delay(25);
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
  Blynk.virtualWrite(V4, sensorValues[acc_x]); delay(25);
  Blynk.virtualWrite(V5, sensorValues[acc_y]); delay(25);
  Blynk.virtualWrite(V6, sensorValues[acc_z]); delay(25);
  Blynk.virtualWrite(V7, sensorValues[mag_x]); delay(25);
  Blynk.virtualWrite(V8, sensorValues[mag_y]); delay(25);
  Blynk.virtualWrite(V9, sensorValues[mag_z]); delay(25);
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
  Blynk.virtualWrite(V10, sensorValues[soil_temp]); delay(25);
  Blynk.virtualWrite(V11, sensorValues[soil_hum]); delay(25);
}

// Управление освещением с Blynk
BLYNK_WRITE(V100)
{
  light_control = param.asInt();
  Serial.print("Light power: ");
  Serial.println(light_control);
  pcf8574.write(0, light_control);
}

// Управление поливом с Blynk
BLYNK_WRITE(V101)
{
  pump_control = param.asInt();
  Serial.print("Pump power: ");
  Serial.println(pump_control);
  pcf8574.write(1, pump_control);
}

// Управление вентиляцией с Blynk
BLYNK_WRITE(V102)
{
  air_control = param.asInt();
  Serial.print("Air cooler power: ");
  Serial.println(air_control);
  pcf8574.write(2, air_control);
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

