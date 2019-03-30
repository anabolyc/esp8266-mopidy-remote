#include <user_config.h>
#include "application.h"
#include "wifimanager.h"
#include "webserver.h"
#include "ir.h"
#include <AppSettings.h>

#define SERIAL_DEBUG true

void onConnected() {
	WebServer::start();
	IR::start();
}

void onReady() {
	WifiManager::start(onConnected);
}

void init()
{
	spiffs_mount();

	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(SERIAL_DEBUG);

	AppSettings.load();
	
	WifiManager::configAccessPoint(true);

	System.onReady(onReady);
}