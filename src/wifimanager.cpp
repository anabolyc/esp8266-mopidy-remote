#include <ESP8266WiFi.h>
#include "wifimanager.h"

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PWD;

#include <Timer.h>
Timer* _led_timer = NULL;
bool savedState1 = false;

WiFiEventHandler connectSuccessHandler;
WiFiEventHandler connectFailHandler;
WiFiEventHandler gotIPHandler;

std::function<void()> _s_callback;
std::function<void()> _f_callback;

void WifiManager::blinkWifiLed(bool enable) {
	if (enable) {
		if (savedState1 == false) {
			_led_timer->after(WIFI_LED_OFF, wifiLedOn);
			savedState1 = true;
		}
	} else {
		if (savedState1 == true) {
			digitalWrite(LED_BLU_PIN, HIGH);
			savedState1 = false;	
		}
	}
}

void WifiManager::gotIP(const WiFiEventStationModeGotIP& evt)
{
	Serial.print(F("Got IP: "));
	Serial.print(evt.ip);
	Serial.print('/');
	Serial.print(evt.mask);
	Serial.print('/');
	Serial.print(evt.gw);
	Serial.println();
	_s_callback();
}

void WifiManager::connectFail(const WiFiEventStationModeDisconnected& evt)
{
	Serial.print(F("Wifi disconnected: "));
	Serial.print(evt.ssid);
	Serial.print(F(", reason: "));
	Serial.println(evt.reason);
	WifiManager::blinkWifiLed(true);
	_f_callback();
}

void WifiManager::connectSuccess(const WiFiEventStationModeConnected& evt)
{
	Serial.print(F("Connected to: "));
	Serial.println(evt.ssid);
	WifiManager::blinkWifiLed(false);
}

void WifiManager::wifiLedOn() {
	digitalWrite(LED_BLU_PIN, LOW);
	_led_timer->after(WIFI_LED_ON, wifiLedOff);
}

void WifiManager::wifiLedOff() {
	digitalWrite(LED_BLU_PIN, HIGH);
	_led_timer->after(WIFI_LED_OFF, wifiLedOn);
}

void WifiManager::start(Timer* timer, std::function<void()> success_callback, std::function<void()> disconnect_callback) {
	_s_callback = success_callback;
	_f_callback = disconnect_callback;
	_led_timer = timer;
    
	pinMode(LED_BLU_PIN, OUTPUT);
	digitalWrite(LED_BLU_PIN, HIGH);

	WiFi.persistent(true);
	WiFi.mode(WIFI_STA);
  	WiFi.begin(ssid, password);

	connectSuccessHandler = WiFi.onStationModeConnected(&connectSuccess);
 	connectFailHandler = WiFi.onStationModeDisconnected(&connectFail);
	gotIPHandler = WiFi.onStationModeGotIP(&gotIP);
}
