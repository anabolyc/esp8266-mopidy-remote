#ifndef _MOPIDY_H_
#define _MOPIDY_H_

#define MOPIDY_HOST    "http://192.168.1.85:6680"

#define VOLUME_STEP    3

#define REPLY_STOPPED  "stopped"
#define REPLY_PLAYING  "playing"
#define REPLY_PAUSED   "paused"

#define LED_RED_PIN 16
#define LED_BLU_PIN  2
#define LED_GRN_PIN  4

#define LED_OFF 250
#define PLAYLIST_RELOAD 2*60*1000

typedef struct _bind_02 {
  http_method method;
  String path;
  String payload;
} RequestBind;

class Mopidy {
public:
    static void start();

    static void toggleState();
    static void play();
    static void stop();
    static void next();
    static void prev();
    static void volumeUp();
    static void volumeDown();
    static void toggleMute();
    static void playTrackNo(uint8_t num);

    static bool isConnected;

private:
    static HttpClient httpClient;
    static Timer *led_timer;
    static Timer *playlist_timer;
    static bool volume_in_prog;
    static int8_t volume_requested;

    static void flashRedLed();
    static void redLedOff();

    static void flashGrnLed();
    static void grnLedOff();

    static void loadPlaylist();
    static void doRequest(const RequestBind *request, RequestCompletedDelegate callback, size_t size = 1024);
    
    static int onRequestComplete(HttpConnection &connection, bool success);
    static int onGetStateComplete(HttpConnection& connection, bool success);
    static int onGetMuteComplete(HttpConnection& connection, bool success);
    static int onGetVolumeUpComplete(HttpConnection& connection, bool success);
    static int onGetVolumeDownComplete(HttpConnection& connection, bool success);
    
    static int onGetPlaylists(HttpConnection &connection, bool success);
    static int onPlaylistLoaded(HttpConnection &connection, bool success);
    static int onTracklistAdded(HttpConnection &connection, bool success);

    static JsonVariant& getReplyPayload(const char *replyText);
    static String getReplyPayloadAsString(const char *replyText);
    
    static void debug_resp(HttpConnection &connection, String reply);
};

#endif