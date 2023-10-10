#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Arduino_JSON.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#include <Preferences.h>
Preferences preferences;
const char* serverName = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_state&board=";
const char* getAlarm = "https://giacongpcb.vn/esp-outputs-action.php?action=getAlarm&board=";
const char* getDelay = "https://giacongpcb.vn/esp-outputs-action.php?action=getDelay3&board=";
const char* serverName2 = "https://giacongpcb.vn/esp-outputs-action.php?action=outputs_name&board=";
const long interval = 80;
unsigned long previousMillis = 0;
String outputsState;
std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
HTTPClient https;
HTTPClient https3;
HTTPClient https2;
String macAdd, nameESP;
int numErrCon = 0;
long tmCheck;
int IO[] = { 14, 12, 13, 15
};
int valGPIO[] = {
  -1, -1, -1, -1
};
int valDelay[] = {
  0, 0, 0, 0
};
long tmDelay[] = {
  0, 0, 0, 0
};
bool boolDelay[] = {
  false, false, false, false
};
int arGPIO[] = {
  -1, -1, -1
};
int delayPress = 50;
bool IssLoad = false;
int delayHours = 1000;
int Hour = 0;
int Min = 0;
int Second = 0; String Day = "";
bool IsOn = false;
bool IsHour = false; long tmChange;
unsigned long key_value = 0;
WiFiServer server(80);
const char* ssid = "Bee R&D";
const char* password = "beeau$beeau";
int numErr = 0; long tmDefault;
JSONVar keys;
WiFiManager wifiManager;
String arDay[7];  String arOn[4]; String arOff[4]; String arHour[7];
String arSP[100];
int countSP = 0; long tmGet;
bool IsON = false;
bool Is1, Is2, Is3, Is4;
bool Is12, Is22, Is32;
int num1, num2, num3, num4, numRS;
int delays = 10;
const int numSig = 5;
int numSpeed = 1;
bool IsRotate = false, IsChange = false;
