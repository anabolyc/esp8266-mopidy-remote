#include <user_config.h>
#include "application.h"
#include "wifimanager.h"
#include "webserver.h"
#include "ir.h"
#include <AppSettings.h>
#include <VarSettings.h>

#define LED_RED_PIN 16 // GPIO16
#define SERIAL_DEBUG true

WebServer webserver;
WifiManager wifimanager;
IR ir;

void onReady() {
	wifimanager.start();
	webserver.start();
	ir.start();
}

void init()
{
	spiffs_mount();

	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(SERIAL_DEBUG);

	AppSettings.load();
	VarSettings.load();
	
	WifiManager::configAccessPoint(true);

	System.onReady(onReady);
}