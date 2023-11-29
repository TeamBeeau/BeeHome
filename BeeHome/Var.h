#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Arduino_JSON.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#include <Preferences.h>

#include "GPIO.h"

Preferences preferences;


const char* _server = "http://giacongpcb.vn/beehome/action.php?action=";
const char* _getStates = "getControls_States&board=";
const char* _getDelays = "getControls_Delays&board=";
const char* _getEvents = "getControls_Events&board=";
const char* _setControls = "setControls&board=";
const char* _setRequest = "setRequest&board=";
const char* _getUsers = "getUsers&board=";
const char* _getHour = "getHour";
const char* _setNew = "setNew&board=";
const char* _setLogin = "setLogin&board=";

const char* serverName = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_state&board=";
const char* getAlarm = "https://giacongpcb.vn/esp-outputs-action.php?action=getAlarm&board=";
const char* getDelay = "https://giacongpcb.vn/esp-outputs-action.php?action=getDelay3&board=";
const char* serverName2 = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_name&board=";

#if NameDevice ==  Outlet3
	const char* boardType = "O03";
	const char* boardStates = "0;0;0;";
	const char* boardDelays = "0;0;0;";
#elif NameDevice == Fan1
	const char* boardType = "F01";
	const char* boardStates = "0;0;";
	const char* boardDelays = "0;0";

#endif


const long interval = 80;
unsigned long previousMillis = 0;


//WiFiClient client;
//HTTPClient https;

String macAdd;
String _User;

bool isStateChange = false;
unsigned long looptime1 = 0;
unsigned long looptime2 = 0;
unsigned long looptime3 = 0;

void update();
void getStates();
void getDeslays();
int nCharInStr(String s, char c);
int* getArrInt(String s, char c);
String replaceAll(String s, char c);
String getRequest(String Url);
