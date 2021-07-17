#ifndef _MOPIDY_H_
#define _MOPIDY_H_

#include <Timer.h>

// #define MOPIDY_HOST "192.168.1.41"
// #define MOPIDY_PORT 6680

#define VOLUME_STEP 3

#define REPLY_STOPPED "stopped"
#define REPLY_PLAYING "playing"
#define REPLY_PAUSED "paused"

#define LED_RED_PIN 16
#define LED_BLU_PIN 2
#define LED_GRN_PIN 4

#define LED_OFF 250
#define PLAYLIST_LOAD 3 * 1000         // 3 sec
#define PLAYLIST_RELOAD 15 * 60 * 1000 // 15 min
// #define PLAYLIST_OFFSET 2
// #define PLAYLIST_COUNT 2

typedef struct _bind_02
{
  String path;
  String payload;
} RequestBind;

void doRequest(const RequestBind *request, std::function<void(int, String)> callback, size_t size = 1024);
String getReplyPayloadAsString(String replyText);
void flashRedLed();
void flashGrnLed();

class Mopidy
{
public:
  static void start(Timer *timer);

  static void toggleState();
  static void play();
  static void stop();
  static void next();
  static void prev();
  static void volumeUp();
  static void volumeDown();
  static void toggleMute();
  static void togglePlaylist();
  static void playTrackNo(uint8_t num);

  static bool isConnected;
  static uint8_t playlistIndex;
  static uint8_t loadPlaylistIndex;

private:
  static void loadPlaylist();

  static void onRequestComplete(int status, String response);
  static void onGetStateComplete(int status, String response);
  static void onGetMuteComplete(int status, String response);
  static void onGetVolumeUpComplete(int status, String response);
  static void onGetVolumeDownComplete(int status, String response);

  static void onGetPlaylists(int status, String response);
  static void onPlaylistLoaded(int status, String response);
  static void onTracklistAdded(int status, String response);
};

#endif