#include "Bee_WiFi_manager.h"


Preferences _WiFin;

#define Debug 0


ESP8266WebServer _httpServer(80);
WebSocketsServer _webSocket = WebSocketsServer(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t lenght);
void handleWebRequests();
void isort(int* a, int n);
void Scan();
void handleRoot();
bool loadFromSpiffs(String path);

void handleRoot() {

#if (Debug == 1)
    Serial.println("handleRoot");
#endif

    //String s = MAIN_page;
    //httpServer.send(200, "text/html", s);
    _httpServer.sendHeader("Location", "/manual.html", false);   //Redirect to our html web page
    _httpServer.send(302, "text/plane", "");
}

bool toge = false;
String setup_string = "";
byte num_ = 0;

bool loadFromSpiffs(String path) {

#if (Debug == 1)
    Serial.println("loadFromSpiffs");
#endif

    String dataType = "text/plain";
    if (path.endsWith("/")) path += "index.htm";

    if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".html")) dataType = "text/html";
    else if (path.endsWith(".htm")) dataType = "text/html";
    else if (path.endsWith(".css")) dataType = "text/css";
    else if (path.endsWith(".js")) dataType = "application/javascript";
    else if (path.endsWith(".png")) dataType = "image/png";
    else if (path.endsWith(".gif")) dataType = "image/gif";
    else if (path.endsWith(".jpg")) dataType = "image/jpeg";
    else if (path.endsWith(".ico")) dataType = "image/x-icon";
    else if (path.endsWith(".xml")) dataType = "text/xml";
    else if (path.endsWith(".pdf")) dataType = "application/pdf";
    else if (path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = LittleFS.open(path.c_str(), "r");
    if (_httpServer.hasArg("download")) dataType = "application/octet-stream";
    if (_httpServer.streamFile(dataFile, dataType) != dataFile.size()) {
    }

    dataFile.close();
    return true;
}

void handleWebRequests() {

#if Debug == 1
    Serial.print("handleWebRequests: ");
    Serial.println(_httpServer.uri());
#endif
    String cmd = _httpServer.uri().substring(0, 6);
    cmd.toLowerCase();
    if (_httpServer.uri().substring(0, 6) == "/user:" ||
        _httpServer.uri().substring(0, 6) == "/user=") {
        String data_ = _httpServer.uri().substring(6);
        int index_ = data_.indexOf(";");
        _WiFin.putString("User", data_.substring(0, index_));
        handleRoot();
        return;
    }

    if (loadFromSpiffs(_httpServer.uri())) return;
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += _httpServer.uri();
    message += "\nMethod: ";
    message += (_httpServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _httpServer.args();
    message += "\n";
    for (uint8_t i = 0; i < _httpServer.args(); i++) {
        message += " NAME:" + _httpServer.argName(i) + "\n VALUE:" + _httpServer.arg(i) + "\n";
    }
    _httpServer.send(404, "text/plain", message);
}

void isort(int* a, int n)
{
    for (int i = 1; i < n; ++i)
    {
        int j = a[i];
        int k;
        for (k = i - 1; (k >= 0) && (j < a[k]); k--)
        {
            a[k + 1] = a[k];
        }
        a[k + 1] = j;
    }
}

void Scan() {
    String ss;
    int n = WiFi.scanNetworks();

#if (Debug == 1)
    Serial.print("Scan: ");
    Serial.print(n);
#endif

    if (n > 0) {
        int* q = new int[n];
        for (int i = 0; i < n; i++) {
            if (WiFi.RSSI(i) <= -100) { q[i] = 0; }
            else if (WiFi.RSSI(i) >= -50) { q[i] = 100; }
            else { q[i] = 2 * (WiFi.RSSI(i) + 100); }
        }
        isort(q, n);
        ss = "ADD:";
        for (int i = n - 1; i >= 0; i--) {
            ss += WiFi.SSID(i);
            ss += ",";
            ss += (String)q[i];
            ss += ",";
            if (WiFi.encryptionType(i) != ENC_TYPE_NONE) ss += "1;";
            else ss += "0;";
        }
        _webSocket.sendTXT(num_, ss);
    }

#if Debug == 1
    Serial.println(ss);
#endif 
}




void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t lenght) {

#if (Debug == 1)
    Serial.println("webSocketEvent");
#endif
    switch (type) {
    case WStype_DISCONNECTED: {

        break;
    }
    case WStype_CONNECTED: {

        break;
    }
    case WStype_TEXT:
        if (lenght > 0) {
            if (setup_string.length() < 1) {
                setup_string = String((const char*)payload);
                num_ = num;
            }
#if (Debug == 1)
            Serial.println();
            Serial.print(num);
            Serial.print(": ");
            Serial.write((const char*)(payload), (lenght));
            Serial.println();
#endif
        }
        break;
    }
}

WifiManager::WifiManager(int led_blink, bool inv) {
    led_stt = led_blink;
    pinMode(led_stt, OUTPUT);
    _inv = inv;
    _WiFin.begin("_myWifin", false);
    _time_out = 100000;
    _localIP = IPAddress(192, 168, 4, 1);
}

void WifiManager::resetSettings() {
    _WiFin.putString("WifiPass", "");
    _WiFin.putString("WifiSSID", "");
}

void WifiManager::setConnectTimeout(unsigned long time_out) {
    _time_out = time_out * 1000;
}

void WifiManager::setLocalIP(IPAddress localIP) {
    _localIP = localIP;
}

bool WifiManager::setHostname(String hostname) {
    return WiFi.hostname( hostname);
}

String WifiManager::getWiFiPass() {
    return _WiFin.getString("WifiPass", "");
}

String WifiManager::getWiFiSSID() {
    return _WiFin.getString("WifiSSID", "");
}

String WifiManager::getUser() {
    return _WiFin.getString("User", "");
}

/*
IPAddress WifiManager::getWiFiIP() {
    String s = _WiFin.getString("WifiIP", "");
    byte ad[4] = { 0,0,0,0 };
    if (s.length() > 7) {
        int index = s.indexOf(".");
        int i = 0;
        while (index > 0) {
            char* c = new char[index];
            s.substring(0, index).toCharArray(c,index);
            ad[i] = (byte)(atoi(c));
            s = s.substring(index + 1);
            index = s.indexOf(".");
        }
    }
    return IPAddress(ad[0], ad[1], ad[2], ad[3]);
}
*/




bool WifiManager::autoConnect(char const* apName, char const* apPassword) {
A:
#if Debug == 1
    Serial.print("Wifi connecting... ");
#endif
    String _ssid = _WiFin.getString("WifiSSID", "");
    String _pass = _WiFin.getString("WifiPass", "");
    if (_ssid.length() > 0) {
        _ssid += "\0";
        _pass += "\0";
        if (WiFi.status() != WL_CONNECTED) {
            //WiFi.disconnect();
            WiFi.mode(WIFI_STA);
#if Debug == 1
            Serial.print("Connect to: ");
            Serial.print(_ssid);
            Serial.print("; ");
            Serial.println(_pass);

#endif
            
            String ss = "Bee R&D";
            String ps = "beeau$beeau";
            WiFi.begin(_ssid, _pass);

            while (WiFi.status() != WL_CONNECTED) {
                delay(100);
#if Debug == 1
                Serial.print(".");
#endif
                //toge = !toge;
                //digitalWrite(led_stt, toge);
                digitalWrite(led_stt, HIGH );
                if (millis() - t2 > 55000) {
                    goto B;
                }
            }
#if Debug == 1
            Serial.println("Connected;");
#endif
            digitalWrite(led_stt, LOW);
            delay(100);
            return true;
        }

    }
    B:
    //_WiFi.begin("_wifi", false);
    int n;
    t2 = millis();
    digitalWrite(led_stt, 1);
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
#if Debug == 1
    Serial.println(apName);
#endif
    WiFi.softAP(apName, apPassword);
    
    IPAddress setup_Subnet(255, 255, 255, 0);
    IPAddress setup_Gateway(192, 168, 1, 0);
    LittleFS.begin();
    WiFi.softAPConfig(_localIP, setup_Gateway, setup_Subnet);
    _httpServer.on("/", handleRoot);
    _webSocket.begin();
    _webSocket.onEvent(webSocketEvent);
    _httpServer.onNotFound(handleWebRequests);
    _httpServer.begin();
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    while (1) {
        _httpServer.handleClient();
        _webSocket.loop();

        if (millis() - t2 > 200) {
            t2 = millis();
            toge = !toge;
            digitalWrite(led_stt, toge);
        }
        if (setup_string.length() > 1) {
            String cmd = setup_string.substring(0, 3);
            String data = "";
            int cmd1 = 0;
            if (cmd == "Sca") cmd1 = 1;
            else if (cmd == "SSI") cmd1 = 2;

#if (Debug == 1)
            Serial.print(cmd);
            Serial.print(" ");
            Serial.print(cmd1);
            Serial.print(" ");
#endif

            switch (cmd1) {
            case 1: {
                Scan();
                break;
            }
            case 2: {
                data = setup_string.substring(5);
                int index_ = setup_string.indexOf(",");
                String ssid_ = setup_string.substring(5, index_);
                String pass_ = setup_string.substring(index_ + 1);
#if (Debug == 1)
                Serial.print(" ssid_: ");
                Serial.print(ssid_);
                Serial.print(" pass_: ");
                Serial.println(pass_);
#endif
                _WiFin.putString("WifiSSID", ssid_);
                _WiFin.putString("WifiPass", pass_);
                fis = true;
                t1 = millis();
                break;
            }
            }
            cmd1 = 0;
            cmd = "";
            setup_string = "";
        }
        if (fis) {
            if (millis() - t1 > 1000) {
                WiFi.softAPdisconnect();
                delay(500);
                ESP.restart();
            }
        }
    }
}