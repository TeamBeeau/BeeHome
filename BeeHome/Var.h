#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Arduino_JSON.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <string.h>

#include <Preferences.h>
#include "Bee_WiFi_manager.h"
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
const char* _getHour1 = "getHour1";
const char* _setNew = "setNew&board=";
const char* _setLogin = "setLogin&board=";
const char* _setEvents = "setEvents&board=";

const char* serverName = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_state&board=";
const char* getAlarm = "https://giacongpcb.vn/esp-outputs-action.php?action=getAlarm&board=";
const char* getDelay = "https://giacongpcb.vn/esp-outputs-action.php?action=getDelay3&board=";
const char* serverName2 = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_name&board=";

#if NameDevice ==  Outlet3
	const char* boardType = "O3";
	const char* boardTypes = "socket;socket;socket;";
	const char* boardStates = "0;0;0;";
	const char* boardDelays = "0;0;0;";
	const char* defautNames = "1;2;3;";
#elif NameDevice == Fan1
	const char* boardType = "F1";
	const char* boardTypes = "power;key;rotate;";
	const char* boardStates = "0;1;0;";
	const char* boardDelays = "0;0;0;";
	const char* defautNames = "Power;Speed;Rotate;";

#endif


const long interval = 80;
unsigned long previousMillis = 0;


//WiFiClient client;
//HTTPClient https;

String macAddBoard;
String _User;

bool isEventChange = false;
bool isStateChange = false;
unsigned long looptime1 = 0;
unsigned long looptime2 = 0;
unsigned long looptime3 = 0;
unsigned long looptime4 = 0;
unsigned long looptime5 = 0;
int httpErrCount = 0;




void connect();
void update();
void getStates();
void getDeslays();
void getEvents();
int nCharInStr(String s, char c);
int* getArrInt(String s, char c);
String replaceAll(String s, char c);
String getRequest(String Url);
int _split_(String s, String split_arr[], String delimeter, int max_length);
void check_event();
void loop2();
void checkInput();
#if NameDevice == Fan1
void but_set(int i);
void checkIr();
#endif