#include "Var.h"
#include "GPIO.h"
#include "Bee_WiFi_manager.h"
#include <string.h>


#define _debug

const unsigned long looptime_States = 1000;
const unsigned long looptime_Delays = 10000;
const unsigned long looptime_Events = 5000;
const unsigned long looptime_update = 500;

const char* ssid = "Bee R&D";
const char* password = "beeau$beeau";

WifiManager wm = WifiManager(Blue);




ICACHE_RAM_ATTR void ButtonIsr() {
    unsigned long _t = millis();
    
;    for (int i = 0; i < numIn; i++) {

        if ((digitalRead(but[i].pin) == but[i].pressed))
        {
            if (!but[i].state) {
                but[i].state = true;
                but[i].time = _t;
                out[i].val = out[i].val == 0 ? 1 : 0;
                digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
                out[i].change = true;
                isStateChange = true;
                looptime2 = millis();
                looptime1 = millis();
            }
        }
        else {
            if (but[i].state) {
                if (millis() - but[i].time > 20) {
                    but[i].state = false;
                }
                
            }
        }
    }
}

void setup() {
	
    preferences.begin("myfile", false);
    

#if NameDevice ==  Outlet3

    for (int i = 0; i < numOut; i++)        // read first
        out[i].val = preferences.getUInt(("OUT" + (String)i).c_str(), 0);

    for (int i=0;i<numIn;i++)
        pinMode(in[i], INPUT_PULLUP);
    for (int i=0;i<numOut;i++)
        pinMode(out[i].pin, OUTPUT);
    pinMode(Blue, OUTPUT);
    digitalWrite(Blue, LOW);

        
    for (int i = 0; i < numOut; i++)
    {
        digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
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
    Reconnect:
    //wm.autoConnect("BeeHome");
    WiFi.begin("Bee R&D", "beeau$beeau");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

#ifdef _debug
    Serial.print(wm.getWiFiSSID());
    Serial.print("  ");
    Serial.println(wm.getWiFiPass());
#endif
    CheckAgan:
    if (WiFi.status() == WL_CONNECTED)
    {
        client->setInsecure();
        macAdd = WiFi.macAddress();
        macAdd.replace(":", "_");
        _User = wm.getUser();
#ifdef _debug
        Serial.print(" MAC: ");
        Serial.print(macAdd);
        Serial.print(" ; User: ");
        Serial.println(_User);
#endif
        
        https.begin(*client, String(_server) + String(_getHour));
        int httpResponseCode = https.GET();

        if (httpResponseCode < 0) goto Reconnect;
        else {
            String payload = https.getString();
            if (payload.length() < 5) goto Reconnect;
        }

        https.setURL(String(_server) + String(_getUsers) + macAdd);
        httpResponseCode = https.GET();
        if (httpResponseCode > 0) {
            String payload = https.getString();
            payload = replaceAll(payload, '"');
            
            if (payload == "null" || payload != _User || wm.New()) {
                //new
                https.setURL(String(_server) + String(_setNew) + macAdd +"&Users=" + _User +
                    "&Type=" + boardType + "&States=" + boardStates + "&Delays=" + boardDelays);
                httpResponseCode = https.GET();
                if (httpResponseCode > 0) {
                    String payload = https.getString();
                    if (payload != "Successfully") {
                        goto CheckAgan;
                    }
                }
                else {
                    goto CheckAgan;
                }
            }

#ifdef _debug
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.println(payload);
#endif
        }
        else {
#ifdef _debug
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
#endif
            delay(1000);
            goto CheckAgan;
        }

    }
    else {
        goto Reconnect;
    }

}




void loop() {
    

    if (millis() - looptime1 > looptime_States) {
        unsigned long ti = millis();
        getStates();

#ifdef _debug
        //Serial.print("getStates() time: ");
        //Serial.println(millis() - ti);
#endif
        getDeslays();
#ifdef _debug
        //Serial.print("getDeslays() time: ");
        //Serial.println(millis() - ti);
#endif
        looptime1 = millis();
    }

    if (millis() - looptime2 > looptime_update)
        update();
}

void update() {
    //update IO
    for (int i = 0; i < numOut; i++) {
        if (out[i].change) {
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
            out[i].temp = out[i].val;
        }
        else if (out[i].temp != out[i].val) {
#if NameDevice ==  Outlet3
            out[i].val = out[i].temp;
            digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
#endif
            out[i].change = true;
        }
        if (out[i].change && out[i].delay != 0) {
            out[i].delay = 0;
            out[i].indelay = 0;
            isStateChange = true;
        }
        if (out[i].delay > 0) {
            if (out[i].indelay == 0) {
                out[i].indelay = millis();
            }
            else if (millis() - out[i].indelay > out[i].delay * 60000) {
#if NameDevice ==  Outlet3
                out[i].val = out[i].val == 0 ? 1 : 0;
                out[i].temp = out[i].val;
                digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
                preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
#endif
                out[i].delay = 0;
                out[i].indelay = 0;
                isStateChange = true;
            }
        }

        out[i].change = false;
    }
    if (isStateChange) {
        //update to server
        String sStates = "";
        String sDelays = "";
        for (int i = 0; i < numOut; i++) {
            sStates += String(out[i].val) + ";";
            sDelays += String(out[i].delay) + ";";
        }

        int _agan = 0;
        https.setURL(String(_server) + String(_setControls) + macAdd
                + "&States=" + sStates + "&Delays=" + sDelays);
        Agan:
        int httpGet = https.GET();
        if (httpGet > 0) {
            String payload = https.getString();
            if (payload != "Successfully") {
                _agan++;
                if (_agan < 3)
                    goto Agan;
            }
        }
        isStateChange = false;
        looptime1 = millis();
        looptime2 = millis();
    }
    else {
        if (millis() - looptime2 > looptime_Delays) {
            https.setURL(String(_server) + String(_setRequest) + macAdd);
            int httpGet = https.GET();
            looptime2 = millis();
        }
        
    }
}

void getDeslays() {
    https.setURL(String(_server) + String(_getDelays) + macAdd);
    int httpGet = https.GET();
    if (httpGet > 0) {
        String payload = https.getString();
        int* arr = getArrInt(payload, ';');


#if NameDevice ==  Outlet3
        if (arr[0] == numOut) {
            for (int i = 1; i <= arr[0]; i++) {
                if (!out[i - 1].change) {
                    out[i - 1].delay = arr[i];
                }
            }
        }
#endif



#ifdef _debug
        //Serial.print("HTTP Delays code: ");
        //Serial.print(httpGet);
        //Serial.print(": ");
        //Serial.println(payload);
#endif
    }
    else {

#ifdef _debug
        Serial.print("Error getDelays code: ");
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
        if (arr[0] == numOut) {
            for (int i = 1; i <= arr[0]; i++) {
                if (!out[i - 1].change) {
                    out[i - 1].temp = arr[i];
                }
            }
        }
#endif



#ifdef _debug
        Serial.print("HTTP getStates code: ");
        Serial.print(httpGet);
        Serial.print(": ");
        Serial.println(payload);
#endif
    }
    else {

#ifdef _debug
        Serial.print("Error getStates code: ");
        Serial.println(httpGet);
#endif
    }
}

// arr[0] is sizeof array
int* getArrInt(String s, char c) {
    s = replaceAll(s, '"');
    int n = nCharInStr(s, c);
    int* arr = new int[n + 1];
    int _index = s.indexOf(c);
    int i = 0;
    while (_index > -1) {
        String sub = s.substring(0, _index);
        i++;
        arr[i] = sub.toInt();
        s = s.substring(_index + 1);
        _index = s.indexOf(c);        
    }
    arr[0] = i;
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

String replaceAll(String s, char c) {
    String result = "";
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] != c) result += s[i];
    }
    return result;
}