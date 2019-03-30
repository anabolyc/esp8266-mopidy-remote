#include "webserver.h"
#include "wifimanager.h"
#include <AppSettings.h>

String WebServer::lastModified;
String WebServer::network, WebServer::password;
Timer *WebServer::conn_timer = NULL;

HttpServer server;

void WebServer::start()
{
	if (fileExist(".lastModified"))
	{
		WebServer::lastModified = fileGetContent(".lastModified");
		WebServer::lastModified.trim();
	}

	server.listen(80);

	server.addPath("/", onIndex);
	server.addPath("/wifi", onWifi);
	server.addPath("/ajax/get-networks", onAjaxNetworkList);
	server.addPath("/ajax/connect", onAjaxConnect);

	server.setDefaultHandler(onFile);
	debugf("Web server service has started");
}

void WebServer::onFile(HttpRequest &request, HttpResponse &response)
{
	if (WebServer::lastModified.length() > 0 && request.headers[HTTP_HEADER_IF_MODIFIED_SINCE].equals(WebServer::lastModified))
	{
		response.code = HTTP_STATUS_NOT_MODIFIED;
		return;
	}

	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		if (WebServer::lastModified.length() > 0)
		{
			response.headers[HTTP_HEADER_LAST_MODIFIED] = WebServer::lastModified;
		}

		response.setCache(86400, true);
		response.sendFile(file);
	}
}

void WebServer::onIndex(HttpRequest &request, HttpResponse &response)
{
	bool led = request.getQueryParameter("led") == "on";
	TemplateFileStream *tmpl = new TemplateFileStream("hello.html");
	auto &vars = tmpl->variables();
	vars["IP"] = WifiStation.getIP().toString();
	vars["MAC"] = WifiStation.getMAC();
	response.sendTemplate(tmpl);
}

void WebServer::onWifi(HttpRequest &request, HttpResponse &response)
{
	response.sendFile("wifi.html");
}

void WebServer::onAjaxNetworkList(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();

	json["status"] = (bool)true;

	bool connected = WifiStation.isConnected();
	json["connected"] = connected;
	if (connected)
	{
		// Copy full string to JSON buffer memory
		json["network"] = WifiStation.getSSID();
	}

	JsonArray &netlist = json.createNestedArray("available");
	for (int i = 0; i < WifiManager::networks.count(); i++)
	{
		if (WifiManager::networks[i].hidden)
			continue;
		JsonObject &item = netlist.createNestedObject();
		item["id"] = (int)WifiManager::networks[i].getHashId();
		// Copy full string to JSON buffer memory
		item["title"] = WifiManager::networks[i].ssid;
		item["signal"] = WifiManager::networks[i].rssi;
		item["encryption"] = WifiManager::networks[i].getAuthorizationMethodName();
	}

	response.setAllowCrossDomainOrigin("*");
	response.sendDataStream(stream, MIME_JSON);
}

void WebServer::makeConnection()
{
	WifiStation.enable(true);
	WifiStation.config(network, password);

	AppSettings.ssid = network;
	AppSettings.password = password;
	AppSettings.save();

	network = "";
	delete conn_timer;
}

void WebServer::onAjaxConnect(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();

	String curNet = request.getPostParameter("network");
	String curPass = request.getPostParameter("password");

	bool updating = curNet.length() > 0 && (WifiStation.getSSID() != curNet || WifiStation.getPassword() != curPass);
	// bool connectingNow = WifiStation.getConnectionStatus() == eSCS_Connecting || network.length() > 0;

	// if(updating && connectingNow) {
	// 	debugf("wrong action: %s %s, (updating: %d, connectingNow: %d)", network.c_str(), password.c_str(), updating,
	// 		   connectingNow);
	// 	json["status"] = (bool)false;
	// 	json["connected"] = (bool)false;
	// } else {
	json["status"] = (bool)true;
	if (updating)
	{
		network = curNet;
		password = curPass;
		debugf("CONNECT TO: %s %s", network.c_str(), password.c_str());
		json["connected"] = false;
		conn_timer = new Timer();
		conn_timer->initializeMs(500, makeConnection).startOnce();
	}
	else
	{
		json["connected"] = WifiStation.isConnected();
		debugf("Network already selected. Current status: %s", WifiStation.getConnectionStatusName());
	}
	// }

	// if(!updating'|VC"}'ror"] = WifiStation.getConnectionStatusName();

	response.setAllowCrossDomainOrigin("*");
	response.sendDataStream(stream, MIME_JSON);
}
