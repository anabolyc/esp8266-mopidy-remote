#ifndef _WIFIMANAGER_H_
#define _WIFIMANAGER_H_

#include <Timer.h>
#include <ESP8266WiFi.h>

#define LED_BLU_PIN  2
#define WIFI_LED_ON  50
#define WIFI_LED_OFF 2000

#define WIFI_SSID    "wifi-12-private"
#define WIFI_PWD     "9263777101"

class WifiManager {

public:
    static void start(Timer* t, std::function<void()> success_callback, std::function<void()> disconnect_callback);
protected:
    static void blinkWifiOn();
    static void blinkWifiOff();
    static void wifiLedOn();
    static void wifiLedOff();
private:
    static void connectFail(const WiFiEventStationModeDisconnected& evt);
    static void connectSuccess(const WiFiEventStationModeConnected& evt);
    static void gotIP(const WiFiEventStationModeGotIP& evt);
};

#endif