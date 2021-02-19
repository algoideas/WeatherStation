
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>

#include "ILI9341_SPI.h"

//Mini Grafx
#include <MiniGrafx.h>
#include <Carousel.h>

#include "ArialRounded.h"
#include "icons.h"

#ifdef BITS_PER_PIXEL
#undef BITS_PER_PIXEL
#define BITS_PER_PIXEL 2
#endif

#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_YELLOW 2
#define MINI_BLUE 3

extern MiniGrafx gfx ;

void initTFT();

void drawMainPage();
void drawCurrentWeather();
void drawForecastPage(uint8_t start);
void drawMainPageForecast1(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y);
void drawMainPageForecast2(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y);

void drawTime();
void drawWifiQuality();

void drawAboutPage();

void drawProgress(uint8_t percentage, String text) ;
