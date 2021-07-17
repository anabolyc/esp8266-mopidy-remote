#include <ESP8266WiFi.h>
#include "wifimanager.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PWD;

#include <Timer.h>
Timer *_led_timer = NULL;
bool blinkIsOn = false;

WiFiEventHandler connectSuccessHandler;
WiFiEventHandler connectFailHandler;
WiFiEventHandler gotIPHandler;

std::function<void()> _s_callback;
std::function<void()> _f_callback;

void WifiManager::blinkWifiOn()
{
	if (blinkIsOn == false)
	{
		_led_timer->after(WIFI_LED_OFF, wifiLedOn);
		blinkIsOn = true;
	}
}

void WifiManager::blinkWifiOff()
{
	blinkIsOn = false;
}

void WifiManager::gotIP(const WiFiEventStationModeGotIP &evt)
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

void WifiManager::connectFail(const WiFiEventStationModeDisconnected &evt)
{
	Serial.print(F("Wifi disconnected: "));
	Serial.print(evt.ssid);
	Serial.print(F(", reason: "));
	Serial.println(evt.reason);
	WifiManager::blinkWifiOn();
	_f_callback();
}

void WifiManager::connectSuccess(const WiFiEventStationModeConnected &evt)
{
	Serial.print(F("Connected to: "));
	Serial.println(evt.ssid);
	WifiManager::blinkWifiOff();
}

void WifiManager::wifiLedOn()
{
	if (blinkIsOn == true)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		_led_timer->after(WIFI_LED_ON, wifiLedOff);
	}
	else
	{
		digitalWrite(LED_BLU_PIN, HIGH);
	}
}

void WifiManager::wifiLedOff()
{
	digitalWrite(LED_BLU_PIN, HIGH);
	if (blinkIsOn == true)
		_led_timer->after(WIFI_LED_OFF, wifiLedOn);
}

void WifiManager::start(Timer *timer, std::function<void()> success_callback, std::function<void()> disconnect_callback)
{
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
