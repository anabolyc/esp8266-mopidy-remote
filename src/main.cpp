#include <Arduino.h>
#include <Timer.h>

#include "wifimanager.h"
#include "ir.h"
#include "mopidy.h"

#define BAUD_RATE  115200
Timer* timer = new Timer();     

bool initialized = false;

void onConnected() {
	Mopidy::isConnected = true;
	if (!initialized) {
		IR::start(timer);
		initialized = true;
	}
}

void onDisconnected() {
	Mopidy::isConnected = false;
}

void setup()
{
  Serial.begin(BAUD_RATE);
  Serial.println(F("Starting..."));
  WifiManager::start(timer, onConnected, onDisconnected);
}

void loop() {
  timer->update();
}