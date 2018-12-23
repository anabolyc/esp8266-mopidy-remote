
#define BTN_A 0xE0E036C9
#define BTN_B 0xE0E028D7
#define BTN_C 0xE0E0A857
#define BTN_D 0xE0E06897

typedef struct _bind_02 {
  char* method;
  char* path;
  char* payload;
} RequestBind;

char* REPLY_STOPPED = "\"stopped\"";

char* REPLY_PLAYING = "\"playing\"";

RequestBind CMD_PLAY  = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.play\"}" };

RequestBind CMD_PAUSE  = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.pause\"}" };

RequestBind CMD_STOP  = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.stop\"}" };

RequestBind CMD_GET_STATE = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.playback.get_state\"}" };

RequestBind CMD_GET_VOLUME = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.get_volume\"}" };

//RequestBind CMD_SET_VOLUME = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.set_volume\", \"params\":[%i]}" };

const int params_shift = 73;

RequestBind CMD_SET_VOLUME(int value) {
  if (value < 0)
    value = 0;
  if (value > 100)
    value = 100;

  char* value_str = (char *)malloc(sizeof(char) * 4);
  value_str = itoa(value, value_str, 10);
  RequestBind result = { "POST", "/mopidy/rpc", "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"core.mixer.set_volume\", \"params\":[   ]}" };

  for(int i = 0, l = strlen(value_str); i < 3; i++) {
    Serial.print(value_str[i]);
    Serial.print(" > ");
    Serial.print(result.payload[params_shift + i]);
    Serial.println("");
    result.payload[params_shift + i] = (i < l) ? value_str[i] : ' ';
  }
  free(value_str);
  return result;
}
