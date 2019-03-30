#ifndef _WEBERVER_H_
#define _WEBERVER_H_

#include <SmingCore/SmingCore.h>

class WebServer {

public:
    static void start();
    static String lastModified;

protected:
    static String network, password;

private:
    static void onFile(HttpRequest &request, HttpResponse &response);
    static void onIndex(HttpRequest &request, HttpResponse &response);
    static void onWifi(HttpRequest &request, HttpResponse &response);
    
    static void onAjaxNetworkList(HttpRequest& request, HttpResponse& response);
    static void onAjaxConnect(HttpRequest& request, HttpResponse& response);
    
    static void makeConnection();
    static Timer *conn_timer;
};

#endif