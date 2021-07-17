#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "mopidy.h"

WiFiClient client;
HTTPClient http;

Timer *_timer = NULL;

int8_t volume_requested;
bool volume_in_prog = false;
bool Mopidy::isConnected = false;
uint8_t Mopidy::playlistIndex = 0;
uint8_t Mopidy::loadPlaylistIndex = 0;

RequestBind CMD_GET_PLAYLISTS = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playlists.as_list\"}"};

RequestBind CMD_PLAY = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.play\"}"};
RequestBind CMD_NEXT = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.next\"}"};
RequestBind CMD_PREV = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.previous\"}"};
RequestBind CMD_PAUSE = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.pause\"}"};
RequestBind CMD_STOP = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.stop\"}"};
RequestBind CMD_GET_STATE = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.get_state\"}"};
RequestBind CMD_GET_VOLUME = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.get_volume\"}"};

RequestBind CMD_SET_VOLUME(int value)
{
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;

	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.set_volume\", \"params\":[ ";
	payload.concat(value);
	payload.concat(" ]}");

	return {
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_MUTE = {"/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.get_mute\"}"};

RequestBind CMD_SET_MUTE(bool value)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.set_mute\", \"params\":[ ";
	payload.concat(value ? "true" : "false");
	payload.concat(" ]}");

	return {
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_PLAYLIST_ITEMS(String uri)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playlists.get_items\", \"params\":{ \"uri\": \"";
	payload.concat(uri);
	payload.concat("\" }}");
	return {
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_TRACKLIST_ADD(String uri)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.tracklist.add\", \"params\":{ \"uris\": [ \"";
	payload.concat(uri);
	payload.concat("\" ] }}");
	return {
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_PLAY_NO(String tlid)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.play\", \"params\":{ \"tlid\": ";
	payload.concat(tlid);
	payload.concat(" }}");
	return {
		"/mopidy/rpc",
		payload};
}

char **playlistItems[PLAYLIST_COUNT];
uint8_t playlistItemsCount[PLAYLIST_COUNT];

void Mopidy::start(Timer *timer)
{
	pinMode(LED_RED_PIN, OUTPUT);
	pinMode(LED_BLU_PIN, OUTPUT);
	pinMode(LED_GRN_PIN, OUTPUT);

	for (uint8_t i = 0; i < PLAYLIST_COUNT; i++)
	{
		playlistItemsCount[i] = 0;
		playlistItems[i] = NULL;
	}

	Serial.println(F("Mopidy service has started"));

	http.setReuse(true);

	_timer = timer;
	_timer->after(PLAYLIST_LOAD, loadPlaylist);
	_timer->every(PLAYLIST_RELOAD, loadPlaylist);
}

void Mopidy::loadPlaylist()
{
	if (isConnected)
	{
		Serial.println(F("Loading playlists..."));
		doRequest(&CMD_GET_PLAYLISTS, onGetPlaylists);
	}
	else
	{
		Serial.println(F("Load playlist skipped"));
	}
}

void Mopidy::toggleState()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_GET_STATE, onGetStateComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::toggleMute()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_GET_MUTE, onGetMuteComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::stop()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_STOP, onRequestComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::next()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_NEXT, onRequestComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::prev()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_PREV, onRequestComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::play()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_PLAY, onRequestComplete);
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::volumeUp()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		volume_requested += VOLUME_STEP;
		if (!volume_in_prog)
		{
			volume_in_prog = true;
			doRequest(&CMD_GET_VOLUME, onGetVolumeUpComplete);
		}
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::volumeDown()
{
	if (isConnected)
	{
		digitalWrite(LED_BLU_PIN, LOW);
		volume_requested -= VOLUME_STEP;
		if (!volume_in_prog)
		{
			volume_in_prog = true;
			doRequest(&CMD_GET_VOLUME, onGetVolumeDownComplete);
		}
	}
	else
	{
		flashRedLed();
	}
}

void Mopidy::playTrackNo(uint8_t num)
{
	if (isConnected)
	{
		if (num >= playlistItemsCount[playlistIndex])
		{
			flashRedLed();
			Serial.print(F("Track not found or track list is not loaded yet: "));
			Serial.println(num);
			return;
		}

		digitalWrite(LED_BLU_PIN, LOW);
		RequestBind req = CMD_GET_TRACKLIST_ADD(playlistItems[playlistIndex][num]);
		doRequest(&req, onTracklistAdded, 4096);
	}
	else
	{
		flashRedLed();
	}
}

// ===================   PRIVATE  ===================

void doRequest(const RequestBind *request, std::function<void(int, String)> callback, size_t size)
{
	Serial.print(F(" -> POST: "));
	Serial.print(MOPIDY_HOST);
	Serial.print(':');
	Serial.print(MOPIDY_PORT);
	Serial.print(request->path);
	Serial.print(' ');
	Serial.println(request->payload);

	http.begin(client, MOPIDY_HOST, MOPIDY_PORT, request->path);
	http.addHeader("Content-Type", "application/json");

	int statusCode = http.POST(request->payload);
	if (statusCode > 0)
	{
		String response = http.getString();

		Serial.print(F(" <- "));
		Serial.println(statusCode);
		Serial.println(response);

		callback(statusCode, response);
	}
	else
	{
		Serial.print(F(" <X "));
		Serial.println(http.errorToString(statusCode));
	}

	http.end();
}

void Mopidy::onGetStateComplete(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		String payload = getReplyPayloadAsString(response);

		if (payload.compareTo(REPLY_PLAYING) == 0)
			doRequest(&CMD_PAUSE, onRequestComplete);
		else if (payload.compareTo(REPLY_STOPPED) == 0)
			doRequest(&CMD_PLAY, onRequestComplete);
		else if (payload.compareTo(REPLY_PAUSED) == 0)
			doRequest(&CMD_PLAY, onRequestComplete);
		else
		{
			Serial.println(F("payload unknown, skipping"));
			flashRedLed();
		}
	}
	else
		flashRedLed();
}

void Mopidy::onGetMuteComplete(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		String payload = getReplyPayloadAsString(response);

		if (payload.compareTo("false") == 0)
		{
			RequestBind _req = CMD_SET_MUTE(true);
			doRequest(&_req, onRequestComplete);
		}
		else if (payload.compareTo("true") == 0)
		{
			RequestBind _req = CMD_SET_MUTE(false);
			doRequest(&_req, onRequestComplete);
		}
		else
		{
			Serial.println(F("payload unknown, skipping"));
			flashRedLed();
		}
	}
	else
		flashRedLed();
}

void Mopidy::onGetVolumeUpComplete(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		String payload = getReplyPayloadAsString(response);

		int volume = atoi(payload.c_str());
		if (volume < 100)
		{
			volume += volume_requested;
			volume_requested = 0;
			RequestBind req = CMD_SET_VOLUME(volume);
			doRequest(&req, onRequestComplete);
		}
	}
	else
		flashRedLed();
	volume_in_prog = false;
}

void Mopidy::onGetVolumeDownComplete(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		String payload = getReplyPayloadAsString(response);

		int volume = atoi(payload.c_str());
		if (volume > 0)
		{
			volume += volume_requested;
			volume_requested = 0;
			RequestBind req = CMD_SET_VOLUME(volume);
			doRequest(&req, onRequestComplete);
		}
	}
	else
		flashRedLed();
	volume_in_prog = false;
}

void Mopidy::onRequestComplete(int status, String response)
{
	if (status >= 200 && status < 300)
		flashGrnLed();
	else
		flashRedLed();
}

String getReplyPayloadAsString(String replyText)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(replyText);
	if (root.success())
		return root["result"].as<const char *>();
	else
		return "";
}

// ================= PLAYLIST INIT =================

void Mopidy::onGetPlaylists(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(response);
		JsonArray &result = root["result"].as<JsonArray>();
		if (result.size() > 0)
		{
			String playlistUri = result[PLAYLIST_OFFSET + loadPlaylistIndex]["uri"].as<const char *>();
			RequestBind req = CMD_GET_PLAYLIST_ITEMS(playlistUri);
			doRequest(&req, onPlaylistLoaded, 4096);
		}
	}
}

void Mopidy::onPlaylistLoaded(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(response);
		JsonArray &result = root["result"].as<JsonArray>();

		if (playlistItems[loadPlaylistIndex] != NULL)
		{
			for (uint8_t i = 0; i < playlistItemsCount[loadPlaylistIndex]; i++)
			{
				delete playlistItems[loadPlaylistIndex][i];
			}
			delete playlistItems[loadPlaylistIndex];
		}

		playlistItems[loadPlaylistIndex] = (char **)malloc(result.size() * sizeof(char *));
		playlistItemsCount[loadPlaylistIndex] = result.size();

		Serial.println(F("Adding tracks to track list"));
		for (uint8_t i = 0; i < result.size(); i++)
		{
			String itemUri = result[i]["uri"].as<const char *>();
			playlistItems[loadPlaylistIndex][i] = (char *)malloc((itemUri.length() + 1) * sizeof(char));
			strcpy(playlistItems[loadPlaylistIndex][i], itemUri.c_str());

			Serial.print(i);
			Serial.print('\t');
			Serial.print(itemUri);
			Serial.println();
		}

		if (++loadPlaylistIndex == PLAYLIST_COUNT)
		{
			loadPlaylistIndex = 0;
		} else {
			loadPlaylist();
		}
	}
}

void Mopidy::onTracklistAdded(int status, String response)
{
	if (status >= 200 && status < 300)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(response);
		JsonArray &result = root["result"].as<JsonArray>();
		if (result.size() > 0)
		{
			String tlid = result[0]["tlid"].as<const char *>();
			RequestBind req = CMD_PLAY_NO(tlid);
			doRequest(&req, onRequestComplete);
		}
	}
}

void Mopidy::togglePlaylist()
{
	if (++playlistIndex == PLAYLIST_COUNT)
		playlistIndex = 0;
}

// ================= LEDS FLASHING =================

void redLedOff()
{
	digitalWrite(LED_RED_PIN, LOW);
}

void grnLedOff()
{
	digitalWrite(LED_GRN_PIN, LOW);
}

void flashRedLed()
{
	digitalWrite(LED_BLU_PIN, HIGH);
	digitalWrite(LED_RED_PIN, HIGH);
	_timer->after(LED_OFF, redLedOff);
}

void flashGrnLed()
{
	digitalWrite(LED_BLU_PIN, HIGH);
	digitalWrite(LED_GRN_PIN, HIGH);
	_timer->after(LED_OFF, grnLedOff);
}