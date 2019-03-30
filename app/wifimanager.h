#ifndef _WIFIMANAGER_H_
#define _WIFIMANAGER_H_

#define LED_BLU_PIN  2 // GPIO2
#define WIFI_LED_ON 50
#define WIFI_LED_OFF 2000

// #ifndef WIFI_SSID
// #define WIFI_SSID "wifi-12-private" // Put you SSID and Password here
// #define WIFI_PWD "92637771010"
// #endif

#include <SmingCore/SmingCore.h>
#include <SmingCore/Delegate.h>

typedef Delegate<void(void)> DeviceConnectedDelegate;

class WifiManager {

public:
    static void start(DeviceConnectedDelegate callback);
    static void configAccessPoint(bool enable);
    static BssList networks;

protected:
    static void blinkWifiLed(bool enable);
    static void wifiLedOn();
    static void wifiLedOff();

private:
    static Timer *led_timer;
    static bool savedState1;
    static bool savedState0;

    static void connectFail(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason);
    static void connectSuccess(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason);
    static void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway);

    static void networkScanCompleted(bool succeeded, BssList list);

};

#endif