#include <SmingCore/SmingCore.h>
#include <SmingCore/Network/HttpClient.h>
#include <SmingCore/Data/Stream/LimitedMemoryStream.h>
#include "mopidy.h"

HttpClient Mopidy::httpClient;

Timer *Mopidy::led_timer = NULL;
Timer *Mopidy::playlist_timer = new Timer();

int8_t Mopidy::volume_requested;
bool Mopidy::volume_in_prog = false;
bool Mopidy::isConnected = false;

RequestBind CMD_GET_PLAYLISTS = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playlists.as_list\"}"};

RequestBind CMD_PLAY = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.play\"}"};
RequestBind CMD_NEXT = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.next\"}"};
RequestBind CMD_PREV = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.previous\"}"};
RequestBind CMD_PAUSE = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.pause\"}"};
RequestBind CMD_STOP = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.stop\"}"};
RequestBind CMD_GET_STATE = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.get_state\"}"};
RequestBind CMD_GET_VOLUME = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.get_volume\"}"};

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
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_MUTE = {HTTP_POST, "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.get_mute\"}"};

RequestBind CMD_SET_MUTE(bool value)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.set_mute\", \"params\":[ ";
	payload.concat(value ? "true" : "false");
	payload.concat(" ]}");

	return {
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_PLAYLIST_ITEMS(String uri)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playlists.get_items\", \"params\":{ \"uri\": \"";
	payload.concat(uri);
	payload.concat("\" }}");
	return {
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_TRACKLIST_ADD(String uri)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.tracklist.add\", \"params\":{ \"uri\": \"";
	payload.concat(uri);
	payload.concat("\" }}");
	return {
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_GET_TRACKIST_ADD(String uri)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.tracklist.add\", \"params\":{ \"uri\": \"";
	payload.concat(uri);
	payload.concat("\" }}");
	return {
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

RequestBind CMD_PLAY_NO(String tlid)
{
	String payload = "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.play\", \"params\":{ \"tlid\": ";
	payload.concat(tlid);
	payload.concat(" }}");
	return {
		HTTP_POST,
		"/mopidy/rpc",
		payload};
}

char **playlistItems = NULL;
uint8_t playlistItemsCount = 0;

void Mopidy::start()
{
	pinMode(LED_RED_PIN, OUTPUT);
	pinMode(LED_BLU_PIN, OUTPUT);
	pinMode(LED_GRN_PIN, OUTPUT);

	debugf("Mopidy service has started");

	loadPlaylist();
	playlist_timer->initializeMs(PLAYLIST_RELOAD, Mopidy::loadPlaylist).start();
}

void Mopidy::loadPlaylist() {
	if (isConnected) {
		debugf("loading playlists");
		doRequest(&CMD_GET_PLAYLISTS, onGetPlaylists);
	} else {
		debugf("load playlist skipped");
	}
}

void Mopidy::toggleState()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_GET_STATE, onGetStateComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::toggleMute()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_GET_MUTE, onGetMuteComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::stop()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_STOP, onRequestComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::next()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_NEXT, onRequestComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::prev()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_PREV, onRequestComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::play()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		doRequest(&CMD_PLAY, onRequestComplete);
	} else {
		flashRedLed();
	}
}

void Mopidy::volumeUp()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		Mopidy::volume_requested += VOLUME_STEP;
		if (!volume_in_prog)
		{
			volume_in_prog = true;
			doRequest(&CMD_GET_VOLUME, onGetVolumeUpComplete);
		}
	} else {
		flashRedLed();
	}
}

void Mopidy::volumeDown()
{
	if (isConnected) {
		digitalWrite(LED_BLU_PIN, LOW);
		Mopidy::volume_requested -= VOLUME_STEP;
		if (!volume_in_prog)
		{
			volume_in_prog = true;
			doRequest(&CMD_GET_VOLUME, onGetVolumeDownComplete);
		}
	} else {
		flashRedLed();
	}
}

void Mopidy::playTrackNo(uint8_t num)
{
	if (isConnected) {
		if (num >= playlistItemsCount)
		{
			flashRedLed();
			debugf("Track #%d not found or track list is not loaded yet");
			return;
		}

		digitalWrite(LED_BLU_PIN, LOW);
		RequestBind req = CMD_GET_TRACKLIST_ADD(playlistItems[num]);
		doRequest(&req, onTracklistAdded, 4096);
	} else {
		flashRedLed();
	}
}

// ===================   PRIVATE  ===================

void Mopidy::doRequest(const RequestBind *request, RequestCompletedDelegate callback, size_t size)
{
	HttpRequest *httpRequest = new HttpRequest(URL(MOPIDY_HOST + request->path));
	httpRequest->setMethod(request->method);

	if (request->method == HTTP_POST)
	{
		httpRequest->setBody(request->payload);
	}

	httpRequest->onRequestComplete(callback);
	httpRequest->setResponseStream(new LimitedMemoryStream(size));

	debugf("\t-> (%s) %s",
		   httpRequest->uri.toString().c_str(),
		   request->payload.c_str());
	httpClient.send(httpRequest);
}

int Mopidy::onGetStateComplete(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		String payload = getReplyPayloadAsString(reply.c_str());

		if (payload.compareTo(REPLY_PLAYING) == 0)
			doRequest(&CMD_PAUSE, onRequestComplete);
		else if (payload.compareTo(REPLY_STOPPED) == 0)
			doRequest(&CMD_PLAY, onRequestComplete);
		else if (payload.compareTo(REPLY_PAUSED) == 0)
			doRequest(&CMD_PLAY, onRequestComplete);
		else
		{
			debugf("payload unknown, skipping");
			flashRedLed();
		}
	}
	else
		flashRedLed();
	return 0;
}

int Mopidy::onGetMuteComplete(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		String payload = getReplyPayloadAsString(reply.c_str());

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
			debugf("payload unknown, skipping");
			flashRedLed();
		}
	}
	else
		flashRedLed();
	return 0;
}

int Mopidy::onGetVolumeUpComplete(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		String payload = getReplyPayloadAsString(reply.c_str());

		int volume = atoi(payload.c_str());
		if (volume < 100)
		{
			volume += Mopidy::volume_requested;
			Mopidy::volume_requested = 0;
			RequestBind req = CMD_SET_VOLUME(volume);
			doRequest(&req, onRequestComplete);
		}
	}
	else
		flashRedLed();
	volume_in_prog = false;
	return 0;
}

int Mopidy::onGetVolumeDownComplete(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		String payload = getReplyPayloadAsString(reply.c_str());

		int volume = atoi(payload.c_str());
		if (volume > 0)
		{
			volume += Mopidy::volume_requested;
			Mopidy::volume_requested = 0;
			RequestBind req = CMD_SET_VOLUME(volume);
			doRequest(&req, onRequestComplete);
		}
	}
	else
		flashRedLed();
	volume_in_prog = false;
	return 0;
}

int Mopidy::onRequestComplete(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
		flashGrnLed();
	else
		flashRedLed();
	return 0;
}

void Mopidy::flashRedLed()
{
	digitalWrite(LED_BLU_PIN, HIGH);
	digitalWrite(LED_RED_PIN, HIGH);
	led_timer = new Timer();
	led_timer->initializeMs(LED_OFF, redLedOff).startOnce();
}

void Mopidy::redLedOff()
{
	digitalWrite(LED_RED_PIN, LOW);
	delete led_timer;
}

void Mopidy::flashGrnLed()
{
	digitalWrite(LED_BLU_PIN, HIGH);
	digitalWrite(LED_GRN_PIN, HIGH);
	led_timer = new Timer();
	led_timer->initializeMs(LED_OFF, grnLedOff).startOnce();
}

void Mopidy::grnLedOff()
{
	digitalWrite(LED_GRN_PIN, LOW);
	delete led_timer;
}

void Mopidy::debug_resp(HttpConnection &connection, String reply)
{
	debugf("\t<- (%d) %s",
		   connection.getResponseCode(),
		   reply.c_str());
}

String Mopidy::getReplyPayloadAsString(const char *replyText)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(replyText);
	if (root.success())
		return root["result"].asString();
	else
		return NULL;
}

// ================= PLAYLIST INIT =================

int Mopidy::onGetPlaylists(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(reply.c_str());
		JsonArray &result = root["result"].asArray();
		if (result.size() > 0)
		{
			String playlistUri = result[0]["uri"].asString();
			RequestBind req = CMD_GET_PLAYLIST_ITEMS(playlistUri);
			doRequest(&req, onPlaylistLoaded, 4096);
		}
	}
	return 0;
}

int Mopidy::onPlaylistLoaded(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(reply.c_str());
		JsonArray &result = root["result"].asArray();

		if (playlistItems != NULL) {
			for(uint8_t i = 0; i < playlistItemsCount; i++) {
				delete playlistItems[i];
			}
			delete playlistItems;
		}

		playlistItems = (char **)malloc(result.size() * sizeof(char *));
		playlistItemsCount = result.size();

		for (int8_t i = 0; i < result.size(); i++)
		{
			String itemUri = result[i]["uri"].asString();
			playlistItems[i] = (char *)malloc((itemUri.length() + 1) * sizeof(char));
			strcpy(playlistItems[i], itemUri.c_str());
			debugf("\t\tTrack #%d %s added to track list", i, itemUri.c_str());
		}
	}
	return 0;
}

int Mopidy::onTracklistAdded(HttpConnection &connection, bool success)
{
	String reply = connection.getResponseString();
	debug_resp(connection, reply);
	if (success)
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject &root = jsonBuffer.parseObject(reply.c_str());
		JsonArray &result = root["result"].asArray();
		if (result.size() > 0)
		{
			String tlid = result[0]["tlid"].asString();
			RequestBind req = CMD_PLAY_NO(tlid);
			doRequest(&req, onRequestComplete);
		}
	}
	return 0;
}