
#include <Arduino.h>
#include <string.h>
#include <stdio.h>
#include <ArduinoJson.h>

#include "Weather.h"

StaticJsonDocument<8192> jsonDataBuffer;
HTTPClient httpClient;

String getUrl = URL_BASIC;
String serverResponse;

String currentConditionId;
String currentConditionMain;
String currentConditionText;
String currentConditionIcon;
String currentFeelTemperature;
String currentAirTemperature;
String currentHumidity;
String currentVisibility;
String currentCloud;

ForecastStruct forecastData[MAX_FORECASTS];

const char* getMeteoconIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return sunny;
  if (iconText == "02d" || iconText == "02n") return partlysunny;
  if (iconText == "03d" || iconText == "03n") return partlycloudy;
  if (iconText == "04d" || iconText == "04n") return mostlycloudy;
  if (iconText == "09d" || iconText == "09n") return rain;
  if (iconText == "10d" || iconText == "10n") return rain;
  if (iconText == "11d" || iconText == "11n") return tstorms;
  if (iconText == "13d" || iconText == "13n") return snow;
  if (iconText == "50d" || iconText == "50n") return fog;
  return unknown;
}
const char* getMiniMeteoconIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return minisunny;
  if (iconText == "02d" || iconText == "02n") return minipartlysunny;
  if (iconText == "03d" || iconText == "03n") return minipartlycloudy;
  if (iconText == "04d" || iconText == "04n") return minimostlycloudy;
  if (iconText == "09d" || iconText == "09n") return minirain;
  if (iconText == "10d" || iconText == "10n") return minirain;
  if (iconText == "11d" || iconText == "11n") return minitstorms;
  if (iconText == "13d" || iconText == "13n") return minisleet;
  if (iconText == "50d" || iconText == "50n") return minifog;
  return miniunknown;
}

bool requestDataFromServer()
{
  httpClient.begin(getUrl,SSL_FINGERPRINT);
  int httpCode = httpClient.GET();
  if (httpCode > 0) 
  {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    //判断请求是否成功
    if (httpCode == HTTP_CODE_OK) 
    {
      //读取响应内容
      serverResponse = httpClient.getString();
      Serial.println("Get the data from Internet!");
      // Serial.println(serverResponse);
      httpClient.end();
      return true;
    }
    else return false;
  } 
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
    httpClient.end();
    return false;
  }
  
}

void freshCurrentWeather()
{
  getUrl = URL_BASIC;
  getUrl += "weather?q=";
  getUrl += URL_LOCATION;
  getUrl += "&units=metric";
  getUrl += "&lang=";
  getUrl += URL_LANGUAGE;
  getUrl += "&appid=";
  getUrl += URL_KEY;
  Serial.println("URL Ready");
  Serial.println(getUrl);
  bool requestResult = requestDataFromServer();
  if (requestResult == true) 
  {
    parseCurrentWeatherData();
    Serial.println("Current Weather Fresh Success!");
  }
  else Serial.println("Current Weather Fresh Failed!");
}

void freshForcastWeather()
{
  getUrl = URL_BASIC;
  getUrl += "forecast?q=";
  getUrl += URL_LOCATION;
  getUrl += "&units=metric";
  getUrl += "&cnt=";
  getUrl += String(MAX_FORECASTS);
  getUrl += "&lang=";
  getUrl += URL_LANGUAGE;
  getUrl += "&appid=";
  getUrl += URL_KEY;
  Serial.println("URL Ready");
  Serial.println(getUrl);
  bool requestResult = requestDataFromServer();
  if (requestResult == true) 
  {
    parseForecastWeatherData();
    Serial.println("Forecast Weather Fresh Success!");
  }
  else Serial.println("Forecast Weather Fresh Failed!");
}

void parseCurrentWeatherData()
{
  deserializeJson(jsonDataBuffer,serverResponse);
  currentConditionId = (int)jsonDataBuffer["weather"][0]["id"];
  currentConditionMain = (const char *)jsonDataBuffer["weather"][0]["main"];
  currentConditionText = (const char *)jsonDataBuffer["weather"][0]["description"];
  currentConditionIcon = (const char *)jsonDataBuffer["weather"][0]["icon"];
  currentFeelTemperature =String((float)jsonDataBuffer["main"]["feels_like"],1);
  currentAirTemperature = String((float)jsonDataBuffer["main"]["temp"],1);
  currentHumidity = (int)jsonDataBuffer["main"]["humidity"];
  currentCloud = (int)jsonDataBuffer["clouds"]["all"];
  currentVisibility = (int)jsonDataBuffer["visibility"];
  Serial.print("currentConditionId:");Serial.println(currentConditionId);
  Serial.print("currentConditionMain:");Serial.println(currentConditionMain);
  Serial.print("currentConditionText:");Serial.println(currentConditionText);
  Serial.print("currentConditionIcon:");Serial.println(currentConditionIcon);
  Serial.print("currentFeelTemperature:");Serial.println(currentFeelTemperature);
  Serial.print("currentAirTemperature:");Serial.println(currentAirTemperature);
  Serial.print("currentHumidity:");Serial.println(currentHumidity);
  Serial.print("currentVisibility:");Serial.println(currentVisibility);
  Serial.print("currentCloud:");Serial.println(currentCloud);
}

void parseForecastWeatherData()
{
  deserializeJson(jsonDataBuffer,serverResponse);
  for (int i=0;i<MAX_FORECASTS;i++)
  {
    forecastData[i].timeStamp = (uint32_t)jsonDataBuffer["list"][i]["dt"];
    forecastData[i].airTemperature = String((float)jsonDataBuffer["list"][i]["main"]["temp"],1);
    forecastData[i].feelTemperature = String((float)jsonDataBuffer["list"][i]["main"]["feels_like"],1);
    forecastData[i].humidity = (int)jsonDataBuffer["list"][i]["main"]["humidity"];
    forecastData[i].conditionId = (int)jsonDataBuffer["list"][i]["weather"][0]["id"];
    forecastData[i].conditionMain = (const char *)jsonDataBuffer["list"][i]["weather"][0]["main"];
    forecastData[i].conditionText = (const char *)jsonDataBuffer["list"][i]["weather"][0]["description"];
    forecastData[i].conditionIcon = (const char *)jsonDataBuffer["list"][i]["weather"][0]["icon"];
    forecastData[i].cloud = (int)jsonDataBuffer["list"][i]["clouds"]["all"];
    
    Serial.print("Group");Serial.println(i);
    Serial.print("ConditionId:");Serial.println(forecastData[i].conditionId);
    Serial.print("ConditionMain:");Serial.println(forecastData[i].conditionMain);
    Serial.print("ConditionText:");Serial.println(forecastData[i].conditionText);
    Serial.print("ConditionIcon:");Serial.println(forecastData[i].conditionIcon);
    Serial.print("FeelTemperature:");Serial.println(forecastData[i].feelTemperature);
    Serial.print("AirTemperature:");Serial.println(forecastData[i].airTemperature);
    Serial.print("Humidity:");Serial.println(forecastData[i].humidity);
    Serial.print("Cloud:");Serial.println(forecastData[i].cloud);
  }
}
