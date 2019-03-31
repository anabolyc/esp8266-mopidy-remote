#include <user_config.h>
#include "application.h"
#include "wifimanager.h"
#include "webserver.h"
#include "mopidy.h"
#include "ir.h"
#include <AppSettings.h>

#define SERIAL_DEBUG true

bool initialized = false;

void onConnected() {
	Mopidy::isConnected = true;
	if (!initialized) {
		WebServer::start();
		IR::start();
		initialized = true;
	}
}

void onDisconnected() {
	Mopidy::isConnected = false;
}

void onReady() {
	WifiManager::start(onConnected, onDisconnected);
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