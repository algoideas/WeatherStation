
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>

#include "icons.h"

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_LED   21

#ifndef _WEATHER_H
#define _WEATHER_H

#define UPDATE_MINUTE 30
#define URL_LOCATION "Huizhou"
//#define URL_LANGUAGE "zh_cn"
#define URL_LANGUAGE "en"
#define URL_KEY "" /* 大家请注意, 需要自己从openweathermap申请账号的相关Key */
#define URL_BASIC "https://api.openweathermap.org/data/2.5/"
#define SSL_FINGERPRINT "6c9d1e27f1137bc7b6159013f2d02997a45b3f7e"
#define MAX_FORECASTS 9

struct ForecastStruct
{
  uint32_t timeStamp;
  String airTemperature;
  String feelTemperature;
  String humidity;

  String conditionId;
  String conditionMain;
  String conditionText;
  String conditionIcon;
  String cloud;
};

extern ForecastStruct forecastData[MAX_FORECASTS];

extern String currentConditionId;
extern String currentConditionMain;
extern String currentConditionText;
extern String currentConditionIcon;
extern String currentFeelTemperature;
extern String currentAirTemperature;
extern String currentHumidity;
extern String currentVisibility;
extern String currentCloud;

extern float localTemperature;
extern float localHumidity;


bool requestDataFromServer();
void freshForcastWeather();
void freshCurrentWeather();
void parseCurrentWeatherData();
void parseForecastWeatherData();
const char* getMeteoconIconFromProgmem(String iconText);
const char* getMiniMeteoconIconFromProgmem(String iconText);

#endif
