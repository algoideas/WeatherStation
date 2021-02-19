
#include <Arduino.h>
#include <FS.h>
#include <TFT_Touch.h>

#include "Weather.h"
#include "network.h"
#include "tftUI.h"

#define DOUT 13  /* Data out pin (T_DO) of touch screen */
#define DIN  12  /* Data in pin (T_DIN) of touch screen */
#define DCS  15  /* Chip select pin (T_CS) of touch screen */
#define DCLK 14  /* Clock pin (T_CLK) of touch screen */

TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);

time_t lastUpdateTimestamp;
time_t lastTouchTimestamp;
time_t currentTimestamp;
bool bledOn = true;
bool blastLedOn = true;
int page = 0;

void setup() 
{
    Serial.begin(115200);
    Serial.println();

    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);

    delay(1000);

    initTFT();

    drawProgress(10,"Mounting File System...");
    SPIFFS.begin();
    delay(1000);

    drawProgress(30,"Init Touch...");
    //Touch init
    touch.setCal(0, 4095, 0, 4095, 320, 240, 0);
    touch.setRotation(3);
    delay(1000);

    drawProgress(50,"Connecting WiFi...");
    if (touch.Pressed())
    startWifiWithWebServer();
    else 
    initWifiConnection();

    configTime(8*3600, 0, "ntp1.aliyun.com");

    drawProgress(70, "Updating Current Weather...");
    freshCurrentWeather();
    delay(1000);

    drawProgress(90, "Updating Forecasts...");
    freshForcastWeather();
    delay(1000);

    currentTimestamp = time(NULL);
    lastUpdateTimestamp = currentTimestamp;
    lastTouchTimestamp = currentTimestamp;
}

void loop() 
{
    currentTimestamp = time(NULL);

    if (touch.Pressed())
    {
        bledOn = true;        
        lastTouchTimestamp = currentTimestamp;

        if (blastLedOn != bledOn)
        {
            blastLedOn = bledOn;
            digitalWrite(TFT_LED, HIGH);
        }
        else
        {
            page = (page + 1) % 5;
        }
    }

    if (currentTimestamp - lastTouchTimestamp > 10)
    {
        bledOn = false;
        if (blastLedOn != bledOn)
        {
            blastLedOn = bledOn;
            digitalWrite(TFT_LED, LOW);
        }
    }

    gfx.fillBuffer(MINI_BLACK);

    if (page == 0)
        drawMainPage();
    else if (page == 1) 
        drawForecastPage(0);
    else if (page == 2)
        drawForecastPage(3);
    else if (page == 3)
        drawForecastPage(6);
    else if (page == 4)
        drawAboutPage();
    gfx.commit();

    if (currentTimestamp - lastUpdateTimestamp > UPDATE_MINUTE * 60)
    {
        drawProgress(10, "Updating Time...");
        configTime(8 * 3600, 0, "ntp1.aliyun.com");
        delay(1000);

        drawProgress(50, "Updating Current Weather...");
        freshCurrentWeather();
        delay(1000);

        drawProgress(70, "Updating Forecasts...");
        freshForcastWeather();
        delay(1000);

        Serial.printf("Free mem: %d\n",  ESP.getFreeHeap());
        lastUpdateTimestamp = currentTimestamp;
    }
}

