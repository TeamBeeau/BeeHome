#include "Var.h"
#include "GPIO.h"
#include "Bee_WiFi_manager.h"
#include <string.h>


#define _debug

const unsigned long looptime_States = 500;
const unsigned long looptime_Delays = 10000;
const unsigned long looptime_Events = 5000;
const unsigned long looptime_update = 1000;

const char* ssid = "Bee R&D";
const char* password = "beeau$beeau";

//WifiManager wm = WifiManager(Blue);




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
                looptime3 = millis();
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
    preferences.end();
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
    ESP.wdtEnable(150000);
    while (digitalRead(0) == 1) {
        delay(100);
    };
#endif

#ifdef _debug
    Serial.begin(115200);
    Serial.println("Start");
#endif
    Reconnect:
    //wm.autoConnect("BeeHome");
    //WiFi.begin("Bee R&D", "beeau$beeau");
    //WiFi.begin("Team 2", "Te@m2023");
    WiFi.begin("Team 2", "Te@m2023");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#ifdef _debug
    //Serial.print(wm.getWiFiSSID());
    Serial.print("  ");
    //Serial.println(wm.getWiFiPass());
#endif
    CheckAgan:
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
        //client->setInsecure();
        macAdd = WiFi.macAddress();
        macAdd.replace(":", "_");
        //_User = wm.getUser();
#ifdef _debug
        Serial.print(" MAC: ");
        Serial.print(macAdd);
        Serial.print(" ; User: ");
        Serial.println(_User);
#endif
        WiFiClient client;
        HTTPClient https;
        https.begin(client, String(_server) + String(_getHour));
        int httpResponseCode = -1;
        try {
            httpResponseCode = https.GET();
        }
        catch (...) {}
        if (httpResponseCode < 0) goto Reconnect;
        else {
            String payload = https.getString();
            if (payload.length() < 5) goto Reconnect;
        }

        https.setURL(String(_server) + String(_getUsers) + macAdd);
        try {
            httpResponseCode = https.GET();
        }
        catch (...) {}
        if (httpResponseCode > 0) {
            String payload = https.getString();
            payload = replaceAll(payload, '"');
            
            if (payload == "null" || payload != _User){// || wm.New()) {
                //new
                https.setURL(String(_server) + String(_setNew) + macAdd +"&Users=" + _User +
                    "&Type=" + boardType + "&States=" + boardStates + "&Delays=" + boardDelays);
                try {
                    httpResponseCode = https.GET();
                }
                catch (...) {}
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
            else {
                https.setURL(String(_server) + String(_setLogin) + macAdd);
                try {
                    httpResponseCode = https.GET();
                }
                catch (...) {}
            }
            

#ifdef _debug
            //Serial.print("HTTP Response code: ");
            //Serial.print(httpResponseCode);
            //Serial.println(payload);
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
    //ESP.wdtDisable();
    
    
}




void loop() {
    

    if (millis() - looptime1 > looptime_States) {
        unsigned long ti = millis();
        try {
            getStates();
        }
        catch (...) {
            Serial.println("catch getStates");
        }
#ifdef _debug
        //Serial.print("getStates() time: ");
        //Serial.println(millis() - ti);
#endif
        //delay(50);

        looptime1 = millis();
    }
    if (millis() - looptime3 > looptime_Delays) {
        try{
            getDeslays();
        }
        catch (...) {
            Serial.println("catch getDelays");
        }
#ifdef _debug
        //Serial.print("getDeslays() time: ");
        //Serial.println(millis() - ti);
#endif
        looptime3 = millis();
    }
    if (millis() - looptime2 > looptime_update) {
        try {
            update();
        }
        catch (...) {
            Serial.println("catch update");
        }
        looptime2 = millis();
        
    }
    delay(0);
}

void update() {
    //update IO
    for (int i = 0; i < numOut; i++) {
        if (out[i].change) {
            preferences.begin("myfile", false);
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
            preferences.end();
            out[i].temp = out[i].val;
        }
        else if (out[i].temp != out[i].val) {
#if NameDevice ==  Outlet3
            out[i].val = out[i].temp;
            digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
            preferences.begin("myfile", false);
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
            preferences.end();
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
                preferences.begin("myfile", false);
                preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
                preferences.end();
#endif
                out[i].delay = 0;
                out[i].indelay = 0;
                isStateChange = true;
            }
        }

        out[i].change = false;
    }
    if (isStateChange) {
        isStateChange = false;
        //update to server
        String sStates = "";
        String sDelays = "";
        for (int i = 0; i < numOut; i++) {
            sStates += String(out[i].val) + ";";
            sDelays += String(out[i].delay) + ";";
        }

        int _agan = 0;
    Agan:
        String payload = getRequest(String(_server) + String(_setControls) + macAdd
            + "&States=" + sStates + "&Delays=" + sDelays);
        if (payload.length() > 0) {
            if (payload != "Successfully") {
                _agan++;
                if (_agan < 3) {
                    delay(50);
                    goto Agan;
                }
            }
        }
        looptime1 = millis();
        looptime2 = millis();
        looptime3 = millis();
    }
    else {
        //if (millis() - looptime2 > looptime_Delays) {
            String payload = getRequest(String(_server) + String(_setRequest) + macAdd);
            //looptime2 = millis();
            //delay(20);
            //Serial.println("Last");
        //}
        
    }
}

void getDeslays() {
    String payload = getRequest(String(_server) + String(_getDelays) + macAdd);
    if (payload.length() > 0) {
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

        delete arr;

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
#endif
    }
}

void getStates() {
    String payload = getRequest(String(_server) + String(_getStates) + macAdd);
    if (payload.length() > 0) {
        
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
        delete arr;


#ifdef _debug
        //Serial.print("HTTP getStates code: ");
        //Serial.print(httpGet);
        //Serial.print(": ");
        //Serial.println(payload);
#endif
    }
    else {
#ifdef _debug
        Serial.print("Error getStates code: ");
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

WiFiClient client;
HTTPClient https;
String getRequest(String Url) {
    String payload = "";
    if (WiFi.status() == WL_CONNECTED) {
        //ESP.setDramHeap();
        //delay(50);
        
        //client.setTimeout(100);
        //https.setReuse(false);
        //https.setTimeout(200);
        //https.setRedirectLimit(1);
        https.begin(client, Url.c_str());
        int httpGet = -1;
        
        try {
            httpGet = https.GET();
            if (httpGet == 200)
                payload = https.getString();

            
        }
        catch (...) {
            Serial.println("Catch getRequest");
        }
        if (httpGet < 0) {
#ifdef _debug
            Serial.print(httpGet);
#endif
        }
        try {
            https.DELETE();
            https.end();
        }
        catch (...) {
#ifdef _debug
            Serial.print("Catch https.end");
#endif
        }
        try {
            //client.flush();
            //client.stop();;
            //client-> abort();
            //client.stopAll();
            Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
            //ESP.resetHeap();
            //Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        }
        catch (...) {
#ifdef _debug
            Serial.print("Catch https.stopAll");
#endif
        
        }
    }
    else {
        WiFi.reconnect();
        
#ifdef _debug
        Serial.println("Wifi disconnected");
#endif

    }
    
    return payload;
}
