#include "webserver.h"
#include "wifimanager.h"
#include <AppSettings.h>
#include <VarSettings.h>

String WebServer::lastModified;
String WebServer::network, WebServer::password;
Timer WebServer::conn_timer;

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
	server.addPath("/vars", onVars);
	server.addPath("/ajax/get-networks", onAjaxNetworkList);
	server.addPath("/ajax/connect", onAjaxConnect);
	server.addPath("/ajax/vars", onAjaxVars);

	server.setDefaultHandler(onFile);
	Serial.println("\r\n=== WEB SERVER STARTED ===");
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

void WebServer::onVars(HttpRequest &request, HttpResponse &response)
{
	response.sendFile("vars.html");
}

void WebServer::onAjaxVars(HttpRequest &request, HttpResponse &response)
{
	switch (request.method)
	{
	case HTTP_OPTIONS:
		debugf("onAjaxVars: %s", "OPTIONS");

		response.setAllowCrossDomainOrigin("*");
		response.setHeader("Access-Control-Allow-Methods", "GET,PUT,DELETE");
		response.code = HTTP_STATUS_OK;
		break;
	
	case HTTP_GET:
		debugf("onAjaxVars: %s", "GET");

		response.setAllowCrossDomainOrigin("*");
		response.setContentType(MIME_JSON);
		if (VarSettings.exist())
			response.sendFile(VarSettings.src);
		else
			response.sendString("{}");
		break;
	
	case HTTP_PUT:
	case HTTP_POST:
	{
		debugf("onAjaxVars: %s", "PUT");

		String key = request.getPostParameter("key");
		String value = request.getPostParameter("value");
		debugf("*** new item: %s: %s", key.c_str(), value.c_str());

		VarSettings.add(key, value);
		VarSettings.save();

		response.code = HTTP_STATUS_OK;
		break;
	}
	default:
		debugf("onAjaxVars: %i", request.method);
	}
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
		conn_timer.initializeMs(500, makeConnection).startOnce();
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
