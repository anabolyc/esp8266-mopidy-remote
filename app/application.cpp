#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "wifi-12-private" // Put you SSID and Password here
	#define WIFI_PWD "9263777101"
#endif

#define LED_BLU_PIN 2 // GPIO2
#define SERIAL_DEBUG false

HttpServer server;
int counter = 0;

void onIndex(HttpRequest &request, HttpResponse &response)
{
	counter++;
	bool led = request.getQueryParameter("led") == "on";
	digitalWrite(LED_BLU_PIN, led);

	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	vars["counter"] = String(counter);
	vars["IP"] = WifiStation.getIP().toString();
	vars["MAC"] = WifiStation.getMAC();
	response.sendTemplate(tmpl); // this template object will be deleted automatically
}

void onHello(HttpRequest &request, HttpResponse &response)
{
	response.setContentType(ContentType::HTML);
	// Use direct strings output only for small amount of data (huge memory allocation)
	response.sendString("Sming. Let's do smart things.");
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/hello", onHello);
	server.setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
	Serial.println("==============================\r\n");
}

// Timer downloadTimer;
// HttpClient downloadClient;
// int dowfid = 0;
// void downloadContentFiles()
// {
// 	if (downloadClient.isProcessing()) return; // Please, wait.

// 	if (downloadClient.isSuccessful())
// 		dowfid++; // Success. Go to next file!
// 	downloadClient.reset(); // Reset current download status

// 	if (dowfid == 0)
// 		downloadClient.downloadFile("http://simple.anakod.ru/templates/index.html");
// 	else if (dowfid == 1)
// 		downloadClient.downloadFile("http://simple.anakod.ru/templates/bootstrap.css.gz");
// 	else if (dowfid == 2)
// 		downloadClient.downloadFile("http://simple.anakod.ru/templates/jquery.js.gz");
// 	else
// 	{
// 		// Content download was completed
// 		downloadTimer.stop();
// 		startWebServer();
// 	}
// }

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	// if (!fileExist("index.html") || !fileExist("bootstrap.css.gz") || !fileExist("jquery.js.gz"))
	// {
	// 	// Download server content at first
	// 	downloadTimer.initializeMs(3000, downloadContentFiles).start();
	// }
	// else
	{
		startWebServer();
	}
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	pinMode(LED_BLU_PIN, OUTPUT);

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(SERIAL_DEBUG); // Enable debug output to serial

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP
	WifiEvents.onStationGotIP(gotIP);
}


// #include <user_config.h>
// #include <SmingCore/SmingCore.h>

// #define LED_BLU_PIN 2  // GPIO2
// #define LED_RED_PIN 16 // GPIO16
// #define  IR_OUT_PIN 4  // GPIO4
// #define   IR_IN_PIN 5  // GPIO5

// Timer procTimer;
// bool state = true;

// void blink()
// {
// 	digitalWrite(LED_RED_PIN, state);
// 	digitalWrite(LED_BLU_PIN, state);
	
// 	Serial.println(state ? "on" : "off");
// 	state = !state;
// }

// void init()
// {
// 	pinMode(LED_BLU_PIN, OUTPUT);
// 	pinMode(LED_RED_PIN, OUTPUT);
	
// 	pinMode(IR_OUT_PIN, OUTPUT);
// 	pinMode(IR_IN_PIN, INPUT);

// 	procTimer.initializeMs(1000, blink).start();

// 	Serial.begin(SERIAL_BAUD_RATE);
// 	Serial.println(" >>> INIT ");
// }