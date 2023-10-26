#pragma once

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Preferences.h>
#include <LittleFS.h>

/* Upload file to ESP
	link: https://www.esp8266.com/viewtopic.php?p=96477
*/

/*
install libs
 <WebSocketsServer.h>
 <ESP8266WebServer.h>
 <Preferences.h>
*/

class WifiManager {
public:
	WifiManager(int led_blink = -1, bool inv = false);
	void resetSettings();
	void setConnectTimeout(unsigned long time_out);
	boolean autoConnect(char const* apName, char const* apPassword = "");
	bool setHostname(String hostname);
	void setLocalIP(IPAddress localIP);
	String getWiFiPass();
	String getWiFiSSID();
	String getUser();
	//IPAddress getWiFiIP();
private:
	unsigned long _time_out;
	IPAddress _localIP;
	unsigned long t2, t1, t, t3, t_s, t_ss;
	int led_stt = -1;
	bool _inv;
	bool fis = false;
};

