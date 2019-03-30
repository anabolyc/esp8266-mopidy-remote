#include "wifimanager.h"
#include <AppSettings.h>

Timer *WifiManager::led_timer = NULL;
bool WifiManager::savedState1 = false;
bool WifiManager::savedState0 = false;

DeviceConnectedDelegate _callback;

BssList WifiManager::networks; 

void WifiManager::start(DeviceConnectedDelegate callback) {
    pinMode(LED_BLU_PIN, OUTPUT);
	digitalWrite(LED_BLU_PIN, 1);

	_callback = callback;

    // #ifdef WIFI_SSID
    // WifiStation.enable(true);
	// WifiStation.config(WIFI_SSID, WIFI_PWD);
	// #endif

	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationConnect(connectSuccess);
	WifiEvents.onStationGotIP(gotIP);
	
	if(AppSettings.exist()) {
		debugf("Loading settings: %s", AppSettings.ssid.c_str());
		WifiStation.enable(true);
		WifiStation.config(AppSettings.ssid, AppSettings.password);
	}

	WifiStation.startScan(networkScanCompleted);
}

void WifiManager::gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	debugf("CONNECTED: %s", ip.toString().c_str());
	_callback();
}

void WifiManager::connectFail(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason)
{
	debugf("Disconnected from %s. Reason: %d", ssid.c_str(), reason);
	configAccessPoint(true);
}

void WifiManager::connectSuccess(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason)
{
	debugf("Connected to %s. Reason: %d", ssid.c_str(), reason);
	configAccessPoint(false);
}

void WifiManager::configAccessPoint(bool enable) {
	if (enable) {
		if (savedState0 == false) {
			debugf("Starting access point");
			WifiAccessPoint.enable(enable);
			WifiAccessPoint.config("esp8266-mopidyctl", "", AUTH_OPEN);
            WifiAccessPoint.setIP(IPAddress(192, 168, 2, 1));
			savedState0 = true;
		}	
	} else {
		if (savedState0 == true) {
			WifiAccessPoint.enable(enable);
			debugf("Stopping access point");
			savedState0 = false;
		}
	}
	blinkWifiLed(enable);
}

void WifiManager::blinkWifiLed(bool enable) {
	if (enable) {
		if (savedState1 == false) {
			led_timer = new Timer();
			led_timer->initializeMs(WIFI_LED_OFF, wifiLedOn).startOnce();
			savedState1 = true;
		}
	} else {
		if (savedState1 == true) {
			led_timer->stop();
			delete led_timer;
			digitalWrite(LED_BLU_PIN, 1);
			savedState1 = false;	
		}
	}
}

void WifiManager::wifiLedOn() {
	digitalWrite(LED_BLU_PIN, 0);
	led_timer->initializeMs(WIFI_LED_ON, wifiLedOff).startOnce();
}

void WifiManager::wifiLedOff() {
	digitalWrite(LED_BLU_PIN, 1);
	led_timer->initializeMs(WIFI_LED_OFF, wifiLedOn).startOnce();
}

void WifiManager::networkScanCompleted(bool succeeded, BssList list)
{
	debugf("Network scan finished");

	if(succeeded) {
		for(int i = 0; i < list.count(); i++)
			if(!list[i].hidden && list[i].ssid.length() > 0) {
				networks.add(list[i]);
				debugf(" - %s", list[i].ssid.c_str());
			}
	}
	networks.sort([](const BssInfo& a, const BssInfo& b) { 
		return b.rssi - a.rssi; 
	});
}