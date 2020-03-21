#ifndef _WIFIMANAGER_H_
#define _WIFIMANAGER_H_

#define LED_BLU_PIN  2 // GPIO2
#define WIFI_LED_ON 50
#define WIFI_LED_OFF 2000

// #ifndef WIFI_SSID
// #define WIFI_SSID "wifi-12-private" // Put you SSID and Password here
// #define WIFI_PWD "92637771010"
// #endif

#include <SmingCore.h>
//#include <SmingCore/Delegate.h>

typedef Delegate<void(void)> DeviceConnectedDelegate;

class WifiManager {

public:
    static void start(DeviceConnectedDelegate success_callback, DeviceConnectedDelegate disconnect_callback);
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

    static void connectFail(const String& ssid, MacAddress bssid, WifiDisconnectReason reason);
    static void connectSuccess(const String& ssid, MacAddress bssid, uint8_t channel);
    static void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway);

    static void networkScanCompleted(bool succeeded, BssList list);

};

#endif