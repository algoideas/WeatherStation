
#include <Arduino.h>
#include <FS.h> //  Settings saved to SPIFFS
#include <WiFi.h>
#include <AutoConnect.h>
#include "HTTPUpdateServer.h"

// Setup
#define DEFAULT_AP_SSID "WeatherStation"
#define DEFAULT_AP_PASSWORD "12345678"
#define HOST_NAME "WeatherStation"

// change for different NTP (time servers)
#define NTP_SERVERS "ntp1.aliyun.com", "ntp.ntsc.ac.cn", "2.cn.pool.ntp.org"

void initWifiConnection();
void startWifiWithWebServer();
