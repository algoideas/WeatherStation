#include <Arduino.h>
#include "tftUI.h"
#include "Weather.h"

uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE, // 1
                      ILI9341_YELLOW, // 2
                      0x7E3C
                      }; //3

const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC,TFT_RST);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

Carousel carousel(&gfx, 0, 0, 240, 100);
FrameCallback frames[] = { drawMainPageForecast1, drawMainPageForecast2 };
const int frameCount = 2;

void initTFT()
{
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
  SPI.setHwCs(false);

  gfx.init();
  gfx.fillBuffer(MINI_BLACK);
  gfx.commit();

  digitalWrite(TFT_LED, HIGH);

  carousel.setFrames(frames, frameCount);
  carousel.setTimePerTransition(1000);
  carousel.setTimePerFrame(5000);
  carousel.setTargetFPS(10);
  carousel.disableAllIndicators();
}

void drawMainPage()
{
  carousel.update();
  drawWifiQuality();
  drawTime();
  drawCurrentWeather();
}

void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) 
{
  gfx.setColor(MINI_YELLOW);
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  time_t forecastTime = forecastData[dayIndex].timeStamp;
  time_t currentTime = time(NULL);
  int forecastHours = (forecastTime - currentTime)/3600;
  if (forecastHours == 0) forecastHours = 1;
  
  gfx.drawString(x, y - 15, String(forecastHours) + " Hours Later");

  gfx.setColor(MINI_WHITE);
  gfx.drawString(x, y, forecastData[dayIndex].airTemperature +"°C");

  gfx.drawPalettedBitmapFromPgm(x-25, y + 15, getMiniMeteoconIconFromProgmem(forecastData[dayIndex].conditionIcon));
  
}
void drawMainPageForecast1(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) 
{
  drawForecastDetail(x + 60, y + 165, 0);
  drawForecastDetail(x + 180, y + 165, 1);
}

void drawMainPageForecast2(MiniGrafx *display,  CarouselState* state, int16_t x, int16_t y) 
{
  drawForecastDetail(x + 60, y + 165, 3);
  drawForecastDetail(x + 180, y + 165, 4);
}

void drawCurrentWeather()
{
  gfx.setTransparentColor(MINI_BLACK);

  // String iconName = "/big/";
  // iconName += currentConditionIcon;
  // iconName += ".bmp";
  // gfx.drawBmpFromFile(iconName, 5, 55);
  
  gfx.drawPalettedBitmapFromPgm(5, 55, getMeteoconIconFromProgmem(currentConditionIcon));
  // Weather Text

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_BLUE);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(225, 65, URL_LOCATION);

  gfx.setFont(ArialRoundedMTBold_36);
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
   
  gfx.drawString(225, 78, currentAirTemperature + "°C");

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_YELLOW);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(225, 118, currentConditionText);

}
void drawTime() 
{
  char time_str[11];
  time_t unixTime;
  struct tm *timeinfo;

  unixTime = time(NULL);
  timeinfo = localtime(&unixTime);

  String date = String(1900 + timeinfo->tm_year);
  date += ".";
  date += String(timeinfo->tm_mon+1);
  date += ".";
  date += String(timeinfo->tm_mday);
  date += "  ";
  date += WDAY_NAMES[timeinfo->tm_wday];

  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  //gfx.setFont(ArialRoundedMTBold_14);
  gfx.setFont(ArialMT_Plain_16);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 236, date);

  gfx.setFont(ArialRoundedMTBold_36);
  sprintf(time_str, "%02d:%02d:%02d\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  gfx.drawString(120, 250, time_str);
}

void drawWifiQuality() 
{
  int quality;
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) quality = 0;
  else if(dbm >= -50) quality = 100;
  else quality = 2 * (dbm + 100);
  
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(228, 8, String(quality) + "%");
  
  if (quality>75) 
  {
    gfx.drawVerticalLine(230,17,2);
    gfx.drawVerticalLine(232,15,4);
    gfx.drawVerticalLine(234,13,6);
    gfx.drawVerticalLine(236,11,8);
  }
  else if (quality>50)
  {
    gfx.drawVerticalLine(230,17,2);
    gfx.drawVerticalLine(232,15,4);
    gfx.drawVerticalLine(234,13,6);
    gfx.drawVerticalLine(236,18,1);
  }
  else if (quality>25)
  {
    gfx.drawVerticalLine(230,17,2);
    gfx.drawVerticalLine(232,15,4);
    gfx.drawVerticalLine(234,18,1);
    gfx.drawVerticalLine(236,18,1);
  }
  else 
  {
    gfx.drawVerticalLine(230,17,2);
    gfx.drawVerticalLine(232,18,1);
    gfx.drawVerticalLine(234,18,1);
    gfx.drawVerticalLine(236,18,1);
  }
}

void drawForecastPagePart(int x, int y, int dayIndex)
{
  const int listOffset = 80;
  time_t forecastTime = forecastData[dayIndex].timeStamp;
  time_t currentTime = time(NULL);
  int forecastHours = (forecastTime - currentTime)/3600;
  
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setFont(ArialRoundedMTBold_14);
  
  gfx.drawString(10, y, String(forecastHours) + " Hours Later");
  gfx.drawPalettedBitmapFromPgm(20, 25 + y, getMiniMeteoconIconFromProgmem(forecastData[dayIndex].conditionIcon));

  // String iconName;
  // iconName = "/mini/";
  // iconName += forecastData[dayIndex].conditionIcon;
  // iconName += ".bmp";
  // gfx.drawBmpFromFile(iconName, 20, y + 25);
  //gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_YELLOW);
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.drawString(x + listOffset, y, forecastData[dayIndex].conditionMain);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  
  gfx.setColor(MINI_BLUE);
  gfx.drawString(x, y+15, "AirTemp.");
  gfx.setColor(MINI_WHITE);
  gfx.drawString(x + listOffset, y+15, forecastData[dayIndex].airTemperature + "°C");
  
  gfx.setColor(MINI_BLUE);
  gfx.drawString(x, y + 30, "Humidity");
  gfx.setColor(MINI_WHITE);
  gfx.drawString(x + listOffset, y + 30, String(forecastData[dayIndex].humidity) + "%");
  gfx.setColor(MINI_BLUE);
  gfx.drawString(x, y + 45, "FeelTemp.");
  gfx.setColor(MINI_WHITE);
  gfx.drawString(x + listOffset, y + 45, forecastData[dayIndex].feelTemperature + "°C");

  gfx.setColor(MINI_BLUE);
  gfx.drawString(x, y+60, "Clouds");
  gfx.setColor(MINI_WHITE);
  gfx.drawString(x + listOffset, y+60, forecastData[dayIndex].cloud + "%");
}
void drawForecastPage(uint8_t start)
{
  //gfx.setFont(ArialRoundedMTBold_14);
  gfx.setFont(ArialMT_Plain_24);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_BLUE);
  gfx.drawString(120, 7, "Forecasts");

  for (int i=0; i<3; i++) drawForecastPagePart(80, 45+i*90, start+i);
}

void drawStringAlignmentCenter(uint8_t line, String value, uint16_t color)
{
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(color);
  gfx.drawString(120, line*15, value);
}
void drawAboutPage() 
{
  gfx.fillBuffer(MINI_BLACK);

  gfx.drawBmpFromPgm(myLogo,0,20);
  
  gfx.setFont(ArialRoundedMTBold_14);

  drawStringAlignmentCenter(8,"Internal RAM Usage",MINI_YELLOW);
  String displayString;
  uint32_t usedMemorySize = (ESP.getHeapSize() - ESP.getFreeHeap()) / 1024;
  uint32_t allMemorySize = ESP.getHeapSize() / 1024;
  displayString = String(usedMemorySize);
  displayString += " / ";
  displayString += String(allMemorySize);
  displayString += " kb   ";
  displayString += String(usedMemorySize*100/allMemorySize);
  displayString += "%";
  drawStringAlignmentCenter(9,displayString,MINI_WHITE);

  drawStringAlignmentCenter(10,"SPI RAM Usage",MINI_YELLOW);
  usedMemorySize = (ESP.getPsramSize() - ESP.getFreePsram()) / 1024;
  allMemorySize = ESP.getPsramSize() / 1024;
  if (allMemorySize != 0)
  {
    displayString = String(usedMemorySize);
    displayString += " / ";
    displayString += String(allMemorySize);
    displayString += " kb   ";
    displayString += String(usedMemorySize*100/allMemorySize);
    displayString += "%";
    drawStringAlignmentCenter(11,displayString,MINI_WHITE);
  }
  else drawStringAlignmentCenter(11,"Not Used",MINI_WHITE);

  drawStringAlignmentCenter(12,"Flash Info",MINI_YELLOW);
  displayString = String(ESP.getFlashChipSize() / 1024 / 1024);
  displayString += " MB @";
  displayString += String(ESP.getFlashChipSpeed() /1000 /1000);
  displayString += "MHz";
  drawStringAlignmentCenter(13,displayString,MINI_WHITE);
  
  drawStringAlignmentCenter(14,"WiFi Strength",MINI_YELLOW);
  displayString = String(WiFi.RSSI());
  displayString += " dB";
  drawStringAlignmentCenter(15,displayString,MINI_WHITE);

  drawStringAlignmentCenter(16,"WiFi IP",MINI_YELLOW);
  drawStringAlignmentCenter(17, WiFi.localIP().toString().c_str(), MINI_WHITE);

  uint64_t chipid = ESP.getEfuseMac();
  displayString = String((uint16_t)(chipid>>32),HEX);
  displayString += String((uint32_t)chipid,HEX);
  displayString.toUpperCase();
  drawStringAlignmentCenter(18,"Chip ID",MINI_YELLOW);
  drawStringAlignmentCenter(19,displayString,MINI_WHITE);
}

void drawProgress(uint8_t percentage, String text) 
{
  gfx.fillBuffer(MINI_BLACK);
  gfx.drawBmpFromPgm(myLogo,0,20);
  
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);

  gfx.setColor(MINI_YELLOW);
  gfx.drawString(120, 146, text);
  gfx.setColor(MINI_WHITE);
  gfx.drawRect(10, 168, 240 - 20, 15);
  gfx.setColor(MINI_BLUE);
  gfx.fillRect(12, 170, 216 * percentage / 100, 11);

  gfx.commit();
}
