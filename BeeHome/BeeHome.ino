#include "Var.h"
#include "GPIO.h"
#include "Bee_WiFi_manager.h"
#include <string.h>


#define _debug

const unsigned long looptime_States = 1000;
const unsigned long looptime_Delays = 10000;
const unsigned long looptime_Events = 5000;

const char* ssid = "Bee R&D";
const char* password = "beeau$beeau";

WifiManager wm = WifiManager(Blue);

#if NameDevice ==  Outlet3

int valGPIO[numOut];

#endif




ICACHE_RAM_ATTR void ButtonIsr() {
    unsigned long _t = millis();
    
;    for (int i = 0; i < numIn; i++) {

        if ((digitalRead(but[i].pin) == but[i].pressed))
        {
            if (!but[i].state) {
                but[i].state = true;
                but[i].time = _t;
                valGPIO[i] = valGPIO[i] == 0 ? 1 : 0;
                digitalWrite(out[i].pin, isState(valGPIO[i], out[i].state));
                isStateChange = true;
            }
        }
        else {
            if (but[i].state) {
                if (millis() - but[i].time > 20) {
                    but[i].state = false;
                    //Serial.print(i);
                    //Serial.println(": press");
                }
                
            }
        }
    }
}

void setup() {
	
    preferences.begin("myfile", false);
    

#if NameDevice ==  Outlet3

    for (int i = 0; i < numOut; i++)        // read first
        valGPIO[i] = preferences.getUInt(("OUT" + (String)i).c_str(), 0);

    for (int i=0;i<numIn;i++)
        pinMode(in[i], INPUT_PULLUP);
    for (int i=0;i<numOut;i++)
        pinMode(out[i].pin, OUTPUT);
    pinMode(Blue, OUTPUT);
    digitalWrite(Blue, LOW);

        
    for (int i = 0; i < numOut; i++)
    {
        digitalWrite(out[i].pin, isState(valGPIO[i] , out[i].state));
    }
    for (int i = 0; i < numIn; i++) {
        attachInterrupt(digitalPinToInterrupt(but[i].pin), ButtonIsr, CHANGE);
        but[i].state = false;
    }

#endif

#ifdef _debug
    Serial.begin(115200);
    Serial.println("Start");
#endif

    //wm.autoConnect("BeeHome");
    WiFi.begin("NgocLe", "0972972501");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    Serial.print(wm.getWiFiSSID());
    Serial.print("  ");
    Serial.println(wm.getWiFiPass());

    if (WiFi.status() == WL_CONNECTED)
    {
        client->setInsecure();
        macAdd = WiFi.macAddress();
        macAdd.replace(":", "_");
#ifdef _debug
        Serial.print(" MAC: ");
        Serial.println(macAdd);
#endif
        https.begin(*client, "https://giacongpcb.vn/beehome/action.php?action=getHour");

        unsigned long ti = millis();
        int httpResponseCode = https.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = https.getString();
            Serial.println(payload);
        }
        else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }

        https.setURL("https://giacongpcb.vn/beehome/action.php?action=getHour1");
        httpResponseCode = https.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP 2 Response code: ");
            Serial.println(httpResponseCode);
            String payload = https.getString();
            Serial.println(payload);
        }
        else {
            Serial.print("Error 2 code: ");
            Serial.println(httpResponseCode);
        }
        Serial.print(" time: ");
        Serial.println((millis() - ti));
        // Free resources
        https.end();
    }

}




void loop() {
    if (isStateChange) {
        for (int i = 0; i < numOut; i++)
            preferences.putUInt(("OUT" + (String)i).c_str(), valGPIO[i]);
        isStateChange = false;
    }

    if (millis() - looptime1 > looptime_States) {
        unsigned long ti = millis();
        getStates();
#ifdef _debug
        Serial.print("getStates() time: ");
        Serial.println(millis() - ti);
#endif
        looptime1 = millis();
    }

    if (millis() - looptime2 > looptime_Delays) {
        unsigned long ti = millis();
        getDeslays();
#ifdef _debug
        Serial.print("getDeslays() time: ");
        Serial.println(millis() - ti);
#endif
        looptime2 = millis();
    }

}

void getDeslays() {
    https.setURL(String(_server) + String(_getDelays) + macAdd);
    int httpGet = https.GET();
    if (httpGet > 0) {
        String payload = https.getString();
        int* arr = getArrInt(payload, ';');


#if NameDevice ==  Outlet3

#endif



#ifdef _debug
        Serial.print("HTTP Response code: ");
        Serial.print(httpGet);
        Serial.print(": ");
        Serial.println(payload);
#endif
    }
    else {

#ifdef _debug
        Serial.print("Error code: ");
        Serial.println(httpGet);
#endif
    }
}

void getStates() {
    https.setURL(String(_server) + String(_getStates) + macAdd);
    int httpGet = https.GET();
    if (httpGet > 0) {
        String payload = https.getString();
        int* arr = getArrInt(payload, ';');


#if NameDevice ==  Outlet3

#endif



#ifdef _debug
        Serial.print("HTTP Response code: ");
        Serial.print(httpGet);
        Serial.print(": ");
        Serial.println(payload);
#endif
    }
    else {

#ifdef _debug
        Serial.print("Error code: ");
        Serial.println(httpGet);
#endif
    }
}

int* getArrInt(String s, char c) {
    int n = nCharInStr(s, c);
    int* arr = new int[n];
    int _index = s.indexOf(c);
    int i = 0;
    while (_index > -1) {
        String sub = s.substring(0, _index);
        arr[i] = sub.toInt();
        i++;
        s = s.substring(_index + 1);
        _index = s.indexOf(c);        
    }
#ifdef _debug
    Serial.println();
#endif
    return arr;
}

int nCharInStr(String s, char c) {
    int i = 0;
    int result = 0;
    while (i < s.length()) {
        if (c == s[i])
            result++;
        i++;
    }
    return result;
}