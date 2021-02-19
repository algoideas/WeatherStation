
#include <Arduino.h>
#include "network.h"
#include "tftUI.h"

WebServer httpServer;
AutoConnectConfig Config;
AutoConnect Portal(httpServer);

HTTPUpdateServer httpUpdater;
AutoConnectAux update("/update", "Update");
AutoConnectAux hello;

int wifiTimeout;

static const char AUX_AppPage[] PROGMEM = R"(
{
  "title": "Welcome",
  "uri": "/",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>Weather Station</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "content",
      "type": "ACText",
      "value": "Weather Forecast Station based ESP32"
    }
  ]
}
)";

bool startSmartWifiConfig()
{
    Serial.println("ø™∆Ù÷«ƒ‹≈‰Õ¯:");
    WiFi.beginSmartConfig();

    String disInfo;

    gfx.fillBuffer(MINI_BLACK);
    gfx.setTextAlignment(TEXT_ALIGN_CENTER);
    gfx.setFont(ArialMT_Plain_16);
    gfx.setColor(MINI_YELLOW);
    gfx.drawString(120, 180, "Wifi SmartConfig...");
    gfx.commit();

    for (wifiTimeout = 0; wifiTimeout < 20; wifiTimeout++)
    {
        Serial.print(".");
        delay(500);
        if (WiFi.smartConfigDone())
        {
            Serial.println("≈‰Õ¯≥…π¶");
            Serial.printf("SSID:%s", WiFi.SSID().c_str());
            Serial.printf("PSW:%s", WiFi.psk().c_str());
            return true;
        }
    }

    if (wifiTimeout >= 20) 
    {
        Serial.println( "≈‰Õ¯≥¨ ±!");
    }

    return false;
}

void startWifiWithWebServer()
{
    Serial.println("WebServer start!");

    String disInfo;
    gfx.fillBuffer(MINI_BLACK);
    gfx.setTextAlignment(TEXT_ALIGN_CENTER);
    //gfx.setFont(ArialRoundedMTBold_14);
    gfx.setFont(ArialMT_Plain_16);
    gfx.setColor(MINI_YELLOW);
    gfx.drawString(120, 90, "Web Server Start");
    gfx.drawString(120, 105, "Please Connect Local AP");
    disInfo = "ssid : ";
    disInfo += DEFAULT_AP_SSID;
    gfx.drawString(120, 135, disInfo);
    disInfo = "password : ";
    disInfo += DEFAULT_AP_PASSWORD;
    gfx.drawString(120, 150, disInfo);
    gfx.drawString(120, 180, "And Visit");
    disInfo = "http://";
    disInfo += HOST_NAME;
    disInfo += ".local";
    gfx.drawString(120, 195, disInfo);
    gfx.commit();

    WiFi.mode(WIFI_MODE_APSTA);
    httpUpdater.setup(&httpServer);
    // Load a custom web page for a sketch and a dummy page for the updater.
    hello.load(AUX_AppPage);
    Portal.join({ hello,update });

    //Config.autoReconnect = true;
    Config.immediateStart = true;
    Config.apid = DEFAULT_AP_SSID;
    Config.psk = DEFAULT_AP_PASSWORD;
    Config.hostName = HOST_NAME;
    Config.homeUri = "/_ac";
    Config.bootUri = AC_ONBOOTURI_ROOT;
    Portal.config(Config);
    Portal.begin();

    while (true)
    {
        Portal.handleClient();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println( "Connect Success!" );
            Serial.println(WiFi.localIP());
            break;
        }
    }

    Portal.end();
}

void initWifiConnection()
{
    WiFi.mode(WIFI_MODE_STA);
    WiFi.setHostname(HOST_NAME);
    WiFi.begin();
    //WiFi.waitForConnectResult();

    for (wifiTimeout=0; wifiTimeout<20; wifiTimeout++)
    {
        if( WiFi.status() == WL_CONNECTED ) 
        {
            Serial.println( "Success!" );
            Serial.println(WiFi.localIP());
            break;
        }
        Serial.print ( "." );
        delay (500);
    }

    if (wifiTimeout>=20) 
    {
        if (!startSmartWifiConfig())
        {
            startWifiWithWebServer();
        }
    }
    else
    {
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        Serial.print("subnetMask: ");
        Serial.println(WiFi.subnetMask());

        Serial.print("gateway: ");
        Serial.println(WiFi.gatewayIP());
    }
}

