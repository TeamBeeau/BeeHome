#include "Var.h"
#include "GPIO.h"
#include "Bee_WiFi_manager.h"
#include <string.h>
#include <Ticker.h>

#define _debug

const unsigned long looptime_States = 100;
const unsigned long looptime_Delays = 2000;
const unsigned long looptime_Events = 5000;
const unsigned long looptime_update = 500;
const unsigned long looptime_checkEvent = 10000;

const char* ssid = "Bee R&D";
const char* password = "beeau$beeau";

WifiManager wm = WifiManager(Blue);

void checkInput() {
    for (int i = 0; i < numIn; i++) {
        if ((ReadIO(but[i].pin) == but[i].pressed))
        {
            if (!but[i].state) {
                but[i].state = true;
                but[i].time = millis();
#if NameDevice ==  Outlet3
                out[i].val = out[i].val == 0 ? 1 : 0;
                digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
#elif NameDevice == Fan1
                but_set(i);
                enBuz = true;
#endif
                out[i].change = true;
                isStateChange = true;
                looptime2 = millis();
                //looptime1 = millis();
                looptime3 = millis();
                looptime4 = millis();
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


ICACHE_RAM_ATTR void ButtonIsr() {
#if NameDevice ==  Outlet3
    //checkInput();
#endif
}

#if NameDevice == Fan1
void syncSpeed() {
#ifdef _debug
    static int ci = 0;
    if (ci > 9) {
        Serial.print("0: " + String(out[0].val));
        Serial.print(" 1: " + String(out[1].val));
        Serial.println("  t: " + String(out[0].delay));
        ci = 0;
    }
    else ci++;
#endif
    if (out[0].val == 1 && out[0].delay > 0) {
        if (out[0].delay > 59 * 6) {
            digitalWrite(out6H.pin, isState(true, out6H.state));
            digitalWrite(out3H.pin, isState(false, out3H.state));
            digitalWrite(out1H.pin, isState(false, out1H.state));
        }
        else if (out[0].delay >= 59 * 3) {
            digitalWrite(out6H.pin, isState(false, out6H.state));
            digitalWrite(out3H.pin, isState(true, out3H.state));
            digitalWrite(out1H.pin, isState(false, out1H.state));
        }
        else {
            digitalWrite(out6H.pin, isState(false, out6H.state));
            digitalWrite(out3H.pin, isState(false, out3H.state));
            digitalWrite(out1H.pin, isState(true, out1H.state));
        }
    }
    else {
        digitalWrite(out6H.pin, isState(false, out6H.state));
        digitalWrite(out3H.pin, isState(false, out3H.state));
        digitalWrite(out1H.pin, isState(false, out1H.state));
    }
    //out[0] power
    //out[1] speed
    //out[2] rotate
    if (out[0].val == 0 || out[2].val == 0) {
        digitalWrite(out[2].pin, isState(false, out[2].state));
        //out[2].val = 0;
    }
    else {
        if (out[2].val &&
            (outsped[0].indelay > 0 || outsped[1].indelay > 0 || outsped[2].indelay > 0))
                digitalWrite(out[2].pin, isState(true, out[2].state));
    }
    if (out[0].val) {
        if (Power_indelay <= 6) {
            firston = true;
            if (Power_indelay < 5)
                digitalWrite(outsped[1].pin, isState(true, outsped[1].state));
            else if (out[1].val != 2) 
                digitalWrite(outsped[1].pin, isState(false, outsped[1].state));
            Power_indelay++;
            if (out[1].val == 0) {
                out[1].val = 1;
                out[1].change = true;
                isStateChange = true;
                enBuz = true;
            }
        }
        else {
            
            firston = false;

            
        }
    }
    else {
        Power_indelay = 0;
        firston = false;
    }
    for (int i = 0; i < 3; i++) {
        if (out[1].val == outsped[i].stt && out[0].val == 1 && Power_indelay > 6) {
            if (outsped[i].indelay > 0) {
                digitalWrite(outsped[i].pin, isState(true, outsped[i].state));
            }
            else outsped[i].indelay++;
        }
        else if (outsped[i].indelay != 0) {
            outsped[i].indelay = 0;
            if (!firston)
                digitalWrite(outsped[i].pin, isState(false, outsped[i].state));
        }
    }

    if (enBuz && outbuzz.stt == 0) {
        digitalWrite(outbuzz.pin, isState(true, outbuzz.state));
        outbuzz.stt = 1;
        outbuzz.indelay = 0;
        enBuz = false;
    }
    if (outbuzz.stt == 1) {
        if (outbuzz.indelay < 1) outbuzz.indelay++;
        else {
            digitalWrite(outbuzz.pin, isState(false, outbuzz.state));
            outbuzz.indelay = 0;
            outbuzz.stt = 0;
        }
    }
}

void but_set(int i) {
    if (i == 1) {
        if (out[0].val == 0) {
            out[1].val = 1;
            out[0].val = 1;
        }
        else if (out[1].val == 3) {
            out[1].val = 1;
            out[0].val = 0;
        }
        else {
            out[1].val++;
        }
        out[0].change = true;
        out[1].change = true;
        isStateChange = true;
        enBuz = true;
    }
    else if (i == 0) {
        if (out[2].val) {
            out[2].val = 0;
            out[2].change = true;
            isStateChange = true;
            enBuz = true;
        }
        else if (out[0].val == 1) {
            out[2].val = 1;
            out[2].change = true;
            isStateChange = true;
            enBuz = true;
        }
    }
}

void checkIr() {
    if (irrecv.decode(&results_IR)) {
        // print() & println() can't handle printing long longs. (uint64_t)
#ifdef _debug
        Serial.print("IR: ");
        serialPrintUint64(results_IR.value, HEX);
        Serial.println("");
#endif
        switch (results_IR.value)
        {
        case 0xC101E57B: {//0
            if (out[0].val != 0 || out[1].val != 1) {
                out[1].val = 1;
                out[0].val = 0;
                out[0].change = true;
                out[1].change = true;
                isStateChange = true;
                enBuz = true;
            }
#ifdef _debug
            Serial.println("ir0");
#endif 
            break;
        }
        case 0x9716BE3F: {//1
            if (out[0].val != 1 || out[1].val != 1) {
                out[1].val = 1;
                out[0].val = 1;
                out[0].change = true;
                out[1].change = true;
                isStateChange = true;
                enBuz = true;
            }
#ifdef _debug
            Serial.println("ir1");
#endif 
            break;
        }
        case 0x3D9AE3F7: {//2
            if (out[0].val != 1 || out[1].val != 2) {
                out[1].val = 2;
                out[0].val = 1;
                out[0].change = true;
                out[1].change = true;
                isStateChange = true;
                enBuz = true;
            }
#ifdef _debug
            Serial.println("ir2");
#endif 
            break;
        }
        case 0x6182021B: {//3
            if (out[0].val != 1 || out[1].val != 3) {
                out[1].val = 3;
                out[0].val = 1;
                out[0].change = true;
                out[1].change = true;
                isStateChange = true;
                enBuz = true;
            }
#ifdef _debug
            Serial.println("ir3");
#endif 
            break;
        }
        case 0xE5CFBD7F: {//EQ
            if (out[2].val) {
                out[2].val = 0;
                out[2].change = true;
                isStateChange = true;
                enBuz = true;
            }
            else if (out[0].val == 1) {
                out[2].val = 1;
                out[2].change = true;
                isStateChange = true;
                enBuz = true;
            }
#ifdef _debug
            Serial.println("irEQ");
#endif 
            break;
        }
        case 0xF0C41643: {//200+
            if (out[0].val == 1) {
                if (out[0].delay == 0) {
                    out[0].delay = 60;
                }
                else if (out[0].delay > 59 * 6) {
                    out[0].delay = 0;
                }
                else if (out[0].delay > 59 * 3) {
                    out[0].delay = 60 * 6;
                }
                else {
                    out[0].delay = 60*3;
                }
                out[0].indelay = 0;
                isStateChange = true;
                enBuz = true;
                looptime3 = millis();
            }
            break;
        }
        default:
            break;
        }
        irrecv.resume();
    }
}

#endif


#if numIn==1
uint but_press[1] = { 0 };
#else
uint but_press[2] = { 0,0 };
#endif

Ticker loop_100ms;

void loop2() {
#if numIn==1
    for (int i = 0; i < 1; i++)
#else 
    for (int i = 0; i < 2; i++)
#endif
    {
        if ((ReadIO(but[i].pin) == but[i].pressed))
        {
            if (but_press[i] < 50) but_press[i]++;
        }
        else if (but_press[i] > 0){
            but_press[i] = 0;
        }
    }
#if numIn==1
    if (but_press[0] >= 50)
#else
    if (but_press[0] >= 50 && but_press[1] >= 50)
#endif
    {
        wm.set_Reset();
        delay(500);
        ESP.restart();
    }
    checkInput();
#if NameDevice == Fan1
    checkInput();
    syncSpeed();
    checkIr();
#endif

}

void setup() {
    loop_100ms.attach(0.07, loop2);
#if NameDevice == Fan1
    irrecv.enableIRIn();
#endif

    pinMode(Blue, OUTPUT);
    digitalWrite(Blue, LOW);

    //irrecv.enableIRIn();


    for (int i = 0; i < numIn; i++) {
        pinMode(in[i], INPUT);
    }
#if NameDevice ==  Outlet3	
    for (int i = 0; i < numOut; i++) {
        pinMode(out[i].pin, OUTPUT);
        digitalWrite(out[i].pin, isState(0, out[i].state));
    }
#elif NameDevice == Fan1
    pinMode(outLOW.pin, OUTPUT);
    digitalWrite(outLOW.pin, isState(0, outLOW.state));
    pinMode(outMED.pin, OUTPUT);
    digitalWrite(outMED.pin, isState(0, outMED.state));
    pinMode(outHIG.pin, OUTPUT);
    digitalWrite(outHIG.pin, isState(0, outHIG.state));
    pinMode(out1H.pin, OUTPUT);
    digitalWrite(out1H.pin, isState(0, out1H.state));
    pinMode(out3H.pin, OUTPUT);
    digitalWrite(out3H.pin, isState(0, out3H.state));
    pinMode(out6H.pin, OUTPUT);
    digitalWrite(out6H.pin, isState(0, out6H.state));
    pinMode(outRotate.pin, OUTPUT);
    digitalWrite(outRotate.pin, isState(0, outRotate.state));
    pinMode(outbuzz.pin, OUTPUT);
    digitalWrite(outbuzz.pin, isState(0, outbuzz.state));
#endif

    preferences.begin("myfile", false);
#if NameDevice ==  Outlet3
    for (int i = 0; i < numOut; i++)        // read first
        out[i].val = preferences.getUInt(("OUT" + (String)i).c_str(), 0);
    preferences.end();
        
    for (int i = 0; i < numOut; i++)
    {
        digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
    }
    for (int i = 0; i < numIn; i++) {
        //attachInterrupt(digitalPinToInterrupt(but[i].pin), ButtonIsr, CHANGE);
        but[i].state = false;
    }
    //while (digitalRead(0) == 1) {
    //    delay(100);
    //};
#elif NameDevice == Fan1
    bool powe = preferences.getBool("Power", false);
    uint Sped = preferences.getUInt("Speed", 0);
    bool Rota = preferences.getBool("Rotate", false);
    out[0].val = powe ? 1 : 0;
    out[1].val = Sped;
    out[2].val = Rota ? 1 : 0;
#endif

    ESP.wdtEnable(150000);
#ifdef _debug
    Serial.begin(115200);
    Serial.println("Start");
#endif
    connect();
    
    
}

void connect() {
Reconnect:
    WiFi.disconnect();
    delay(2000);
    wm.autoConnect("BeeHome");
    //WiFi.begin("Bee R&D", "beeau$beeau");
    //WiFi.begin("Team 2", "Te@m2023");
    //WiFi.begin("Ngoc", "0972972501");
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
        if (macAddBoard.length() < 2) macAddBoard = WiFi.macAddress();
        
        macAddBoard.replace(":", "_");
        _User = wm.getUser();
        //_User = "test";
#ifdef _debug
        Serial.print(" MAC: ");
        Serial.print(macAddBoard);
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
            looptime5 = millis();
        }

        https.setURL(String(_server) + String(_getUsers) + macAddBoard);
        try {
            httpResponseCode = https.GET();
        }
        catch (...) {}
        if (httpResponseCode > 0) {
            String payload = https.getString();
            payload = replaceAll(payload, '"');
            if (payload == "null" || payload != _User) {// || wm.New()) {
                //new
                https.setURL(String(_server) + String(_setNew) + macAddBoard + "&Users=" + _User + "&defname=" + String(defautNames) +
                    "&Type=" + String(boardType) + "&States=" + String(boardStates) + "&Delays=" + String(boardDelays) + "&Types=" + String(boardTypes));
                try {
                    httpResponseCode = https.GET();
                }
                catch (...) {}
                if (httpResponseCode > 0) {
                    String payload = https.getString();
                    payload.replace("\"", "");
                    if (payload != "Successfully") {
                        delay(1000);
                        goto CheckAgan;
                    }
                }
                else {
                    delay(1000);
                    goto CheckAgan;
                }
            }
            else {
                https.setURL(String(_server) + String(_setLogin) + macAddBoard);
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
    //irrecv.enableIRIn();
}


void loop() {
    //if (irrecv.decode(&results)) {
    //    // print() & println() can't handle printing long longs. (uint64_t)
    //    serialPrintUint64(results.value, HEX);
    //    Serial.println("");
    //    irrecv.resume();  // Receive the next value
    //}
    if (millis() - looptime1 > looptime_States) {
        unsigned long ti = millis();
        try {
            getStates();
        }
        catch (...) {
#ifdef _debug
            Serial.println("catch getStates");
#endif
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
#ifdef _debug
            Serial.println("catch getDelays");
#endif
        }
#ifdef _debug
        //Serial.print("getDeslays() time: ");
        //Serial.println(millis() - ti);
#endif
        looptime3 = millis();
    }

    if (millis() - looptime4 > looptime_Events) {
        try {
            getEvents();
        }
        catch (...) {
#ifdef _debug
            Serial.println("catch getEvents");
#endif
        }
#ifdef _debug
        //Serial.print("getDeslays() time: ");
        //Serial.println(millis() - ti);
#endif
        looptime4 = millis();
    }

    check_event();

    if (millis() - looptime2 > looptime_update) {
        try {
            update();
        }
        catch (...) {
#ifdef _debug
            Serial.println("catch update");
#endif
        }
        looptime2 = millis();
        
    }
    delay(0);
}

void check_event() {
    if (millis() - looptime5 > looptime_checkEvent) {
        String payload;
        String ds[3];
        int d[3];
        bool inday;
        for (int i = 0; i < numRq; i++) {
            if (_event[i].eIF.enable) {
                if (payload.length() < 2) {
                    payload = getRequest(String(_server) + String(_getHour1));
                    payload.replace("\"", "");
                    _split_(payload, ds, ",", 3);
                    d[0] = ds[0].toInt(); 
                    d[1] = ds[1].toInt();
                    d[2] = ds[2].toInt();
#ifdef _debug
                    Serial.println("Day:" + String(d[0]) + " hour:" + String(d[1]) + " minute:" + String(d[2]));
#endif
                    
                }
                inday = (_event[i].eIF.day >> (d[0] - 1) & 0x01)  == 1;
                if (_event[i].eIF.day == 0 || inday) {
                    int preal = d[1] * 60 + d[2];
                    if (_event[i].eIF.instate1) {
                        int pset = _event[i].eIF.hour * 60 + _event[i].eIF.minute + _event[i].eIF.delay;
                        if (millis() - _event[i].eIF.time > _event[i].eIF.delay * 60000) {
                            _event[i].eIF.instate1 = false;
                            if (_event[i].eIF.day == 0) {
                                _event[i].eIF.s = "";
                                isEventChange = true;
                            }

                            out[i].val = _event[i].eIF.state2;
#if NameDevice ==  Outlet3
                            digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
#endif
                            out[i].change = true;
                            isStateChange = true;
#if NameDevice == Fan1
                            enBuz = true;
#endif

                        }
                    }
                    else {
                        int pset = _event[i].eIF.hour * 60 + _event[i].eIF.minute;
                        if (pset == preal || ((pset + 1) == preal) && _event[i].eIF.delay > 1){
                            _event[i].eIF.instate1 = true;
                            _event[i].eIF.time = millis();

                            out[i].val = _event[i].eIF.state1;
#if NameDevice ==  Outlet3
                            digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
#endif
                            out[i].change = true;
                            isStateChange = true;
#if NameDevice == Fan1
                            enBuz = true;
#endif

                        }
                    }
                }
            }
        }
        if (isEventChange) {
            String nevent = "";
            for (int i = 0; i < numRq; i++) {
                if (!_event[i].eIF.s.isEmpty()) {
                    if (_event[i].eIF.s.length() > 2)
                        nevent += _event[i].eIF.s + ";";
                }
            }
            int _agan = 0;
            while (_agan < 3) {
                String payload = getRequest(String(_server) + String(_setEvents) + macAddBoard + "&nEvent=" + nevent);
                //Serial.println(String(_server) + String(_setEvents) + macAddBoard + "&nEvent=" + nevent);
                if (payload.length() > 0) {
                    payload.replace("\"", "");
                    if (payload != "Successfully") {
                        _agan++;
                        delay(500);
#ifdef _debug
                        Serial.println("event again");
#endif
                    }
                    else break;
                }
            }
            isEventChange = false;
        }
        looptime5 = millis();
    }
}

void getEvents() {
    String payload = getRequest(String(_server) + String(_getEvents) + macAddBoard);
    //Serial.println("macAddBoard: " + macAddBoard);
    //Serial.println("getEvent: " + String(_server) + String(_getEvents) + macAddBoard);
    //Serial.println("getEvent: " + payload);
    payload.trim();
    payload.replace("\"", "");
    payload.replace("\r", "");
    payload.replace("\n", "");
    for (int _i = 0; _i < numRq; _i++) {
        _event[_i].eIF.enable = false;
        _event[_i].eIF.s = "";
    }
    if (payload.length() > 0) {
        String even[numRq + 1];
        int i = _split_(payload, even, ";", 3);
        while (i >= 0) {
            if (even[i].isEmpty() || even[i].length() < 2) {
                i--;
                continue;
            }
            int k = even[i].indexOf("_");
            if (k < 1) {
                i--;
                continue;
            }
            int cmd = even[i].substring(0, k).toInt();
            String data = even[i].substring(k + 1);
            switch (cmd)
            {
            case 1: { // IF : cmd_id_day,hour,minute_state1_delay_state2;
                k = data.indexOf("_");
                if (k < 1) break;
                int id = data.substring(0, k).toInt() - 1;
                if (id < 0) break;
                data = data.substring(k + 1);
                k = data.indexOf(",");
                _event[id].eIF.day = data.substring(0, k).toInt();
                data = data.substring(k + 1);
                k = data.indexOf(",");
                _event[id].eIF.hour = data.substring(0, k).toInt();
                data = data.substring(k + 1);
                k = data.indexOf("_");
                _event[id].eIF.minute = data.substring(0, k).toInt();
                data = data.substring(k + 1);
                k = data.indexOf("_");
                _event[id].eIF.state1 = data.substring(0, k).toInt();
                data = data.substring(k + 1);
                k = data.indexOf("_");
                _event[id].eIF.delay = data.substring(0, k).toInt();
                data = data.substring(k + 1);
                _event[id].eIF.state2 = data.toInt();
                _event[id].eIF.enable = true;
                _event[id].eIF.s = even[i];
                //Serial.println("id: " + String(id) + " day: " + String(_event[id].eIF.day) + " hour: " + String(_event[id].eIF.hour) + " minute: " + String(_event[id].eIF.minute) + 
                //    " state1: " + String(_event[id].eIF.state1) + " delay: " + String(_event[id].eIF.delay) + " state2: " + String(_event[id].eIF.state2));
                break;
            }
            default:
                break;
            }
            i--;
        }


#ifdef _debug
        //Serial.print("HTTP Events code: ");
        //Serial.print(httpGet);
        //Serial.print(": ");
        //Serial.println(payload);
#endif
    }
    else {
#ifdef _debug
        Serial.println("Error getEvents code: ");
#endif
    }
}

void update() {
    //update IO
    for (int i = 0; i < numRq; i++) {
        if (out[i].change) {
            preferences.begin("myfile", false);
#if NameDevice ==  Outlet3	
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
#elif NameDevice == Fan1
            if (i == 0) {
                preferences.putBool("Power", out[0].val == 0 ? false : true);
            }
            else if (i == 1) {
                preferences.putUInt("Speed", out[1].val);
            }
            else if (i == 2) {
                preferences.putBool("Rotate", out[2].val == 0 ? false : true);
            }
#endif
            preferences.end();
            out[i].temp = out[i].val;
        }
        else if (out[i].temp != out[i].val) {

            out[i].val = out[i].temp;
            digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
            preferences.begin("myfile", false);
#if NameDevice ==  Outlet3
            preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
#elif NameDevice == Fan1
            if (i == 0) {
                preferences.putBool("Power", out[0].val == 0 ? false : true);
            }
            else if (i == 1) {
                preferences.putUInt("Speed", out[1].val);
            }
            else if (i == 2) {
                preferences.putBool("Rotate", out[2].val == 0 ? false : true);
            }
#endif
            preferences.end();
            out[i].change = true;
        }
        if (out[i].change && out[i].delay != 0) {
            out[i].delay = 0;
            out[i].indelay = 0;
            isStateChange = true;
#if NameDevice == Fan1
            enBuz = true;
#endif
        }
        if (out[i].delay > 0) {
            if (out[i].indelay == 0) {
                out[i].indelay = millis();
            }
            else if (millis() - out[i].indelay > out[i].delay * 60000) {

                out[i].val = out[i].val == 0 ? 1 : 0;
                out[i].temp = out[i].val;
                preferences.begin("myfile", false);
#if NameDevice ==  Outlet3
                digitalWrite(out[i].pin, isState(out[i].val, out[i].state));
                preferences.putUInt(("OUT" + (String)i).c_str(), out[i].val);
#elif NameDevice == Fan1
                if (i == 0) {
                    preferences.putBool("Power", out[0].val == 0 ? false : true);
                }
                else if (i == 1) {
                    preferences.putUInt("Speed", out[1].val);
                }
                else if (i == 2) {
                    preferences.putBool("Rotate", out[2].val == 0 ? false : true);
                }
                enBuz = true;
#endif
                preferences.end();
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
        for (int i = 0; i < numRq; i++) {
            sStates += String(out[i].val) + ";";
            sDelays += String(out[i].delay) + ";";
        }
        
        int _agan = 0;
        while (_agan < 3) {
            String payload = getRequest(String(_server) + String(_setControls) + macAddBoard
                + "&States=" + sStates + "&Delays=" + sDelays);
            if (payload.length() > 0) {
                payload.replace("\"", "");
                if (payload != "Successfully") {
                    _agan++;
#ifdef _debug
                    Serial.println("update again: " + String(_agan));
#endif
                    delay(500);

                }
                else break;
            }
        }
        looptime1 = millis();
        looptime2 = millis();
        looptime3 = millis();
        looptime4 = millis();


    }
    else {
        //if (millis() - looptime2 > looptime_Delays) {
            String payload = getRequest(String(_server) + String(_setRequest) + macAddBoard);
            //looptime2 = millis();
            //delay(20);
            //Serial.println("Last");
        //}
        
    }
}

void getDeslays() {
    String payload = getRequest(String(_server) + String(_getDelays) + macAddBoard);
    if (payload.length() > 0) {
        int* arr = getArrInt(payload, ';');
//#if NameDevice ==  Outlet3
        if (arr[0] == numRq) {
            for (int i = 1; i <= arr[0]; i++) {
                if (!out[i - 1].change) {
                    out[i - 1].delay = arr[i];
                }
            }
        }
//#endif

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
    String payload = getRequest(String(_server) + String(_getStates) + macAddBoard);
    if (payload.length() > 0) {
        
        int* arr = getArrInt(payload, ';');
//#if NameDevice ==  Outlet3
        if (arr[0] == numRq) {
            for (int i = 1; i <= arr[0]; i++) {
                if (!out[i - 1].change) {
                    out[i - 1].temp = arr[i];
                }
            }
        }
//#endif
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
    int ag = 0;
    String payload = "";
    if (WiFi.status() == WL_CONNECTED) {
        //ESP.setDramHeap();
        //delay(50);
        
        client.setTimeout(1000);
        https.setReuse(false);
        https.setTimeout(1000);
        //https.setRedirectLimit(1);
        https.begin(client, Url.c_str());
        int httpGet = -1;
        Getagain:
        try {
            httpGet = https.GET();
            if (httpGet == 200) {
                payload = https.getString();
                httpErrCount = 0;
            }
            
        }
        catch (...) {
#ifdef _debug
            Serial.println("Catch getRequest");
#endif
        }
        if (httpGet < 0) {
#ifdef _debug
            Serial.print("getRequest: " + String(ag));
            Serial.print(" : ");
            Serial.println(httpGet);
#endif
            if (ag < 3) {
                ag++;
                delay(200);
                goto Getagain;
            }
            else httpErrCount++;
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
            if (ESP.getFreeHeap() < 3000)
            {
                
#ifdef _debug
                Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
#endif
                wifi_softap_free_station_info();
                ESP.resetFreeContStack();
                ESP.reset();
                
            }
            //ESP.resetHeap();
            //Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        }
        catch (...) {
#ifdef _debug
            Serial.print("Catch https.stopAll");
#endif
        
        }
        if (httpErrCount > 5) {
#ifdef _debug
            Serial.println("httpErrCount > 5");
#endif
            httpErrCount = 0;

            connect();
        }
    }
    else {
        connect();
        
#ifdef _debug
        Serial.println("Wifi disconnected");
#endif

    }
    
    return payload;
}


int _split_(String s, String split_arr[], String delimeter, int max_length) {
    int i = 0;
    int a = s.indexOf(delimeter);
    while (a != -1) {
        split_arr[i] = s.substring(0, a);
        if (a + 1 > s.length() - 1) {
            break;
        }
        s = s.substring(a + 1);
        a = s.indexOf(delimeter);
        i++;
        if (max_length != -1 && i >= max_length) {
            break;
        }
    }
    if (s.length() != 0) {
        split_arr[i] = s;
    }
    return i;
}