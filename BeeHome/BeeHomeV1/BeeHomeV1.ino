#define GPIO1 14
#define GPIO2 12
#define GPIO3 13
#define GPIO4 15
#define Blue 16
#define btn2 0//12
#define btn4 5//5
#define btn3 4////3
#define btn1 2//0

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





String httpGETRequest2() {
    // Send HTTP POST request
    int httpResponseCode = https2.GET();
    String payload = "{}";
    if (httpResponseCode > 0) {
        // Serial.print("HTTP Response code: ");
        // Serial.println(httpResponseCode);
        payload = https2.getString();
    }
    else {
        Serial.print("Error code2: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    return payload;
}

String httpGETRequest3() {
    // Send HTTP POST request
    int httpResponseCode = https3.GET();

    String payload = "{}";

    if (httpResponseCode > 0) {
        // Serial.print("HTTP Response code: ");
        // Serial.println(httpResponseCode);
        payload = https3.getString();
    }
    else {
        Serial.print("Error code2: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    // https.end();

    return payload;
}

void split(String data, char c)
{
    countSP = 0; int r = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (data.charAt(i) == c)
        {
            arSP[countSP] = data.substring(r, i);
            r = (i + 1);
            countSP++;
        }
        if (i == data.length() - 1)
        {
            arSP[countSP] = data.substring(r, i + 1); countSP++;
        }
    }
}

String arSP2[100];
int countSP2 = 0;
bool IsAttach = false;

void split2(String data, char c)
{
    countSP2 = 0; int r = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (data.charAt(i) == c)
        {
            arSP2[countSP2] = data.substring(r, i);
            r = (i + 1);
            countSP2++;
        }
        if (i == data.length() - 1)
        {
            arSP2[countSP2] = data.substring(r, i + 1); countSP2++;
        }
    }
}



int numSpeedOld = 0;

void SendReq(String key, String value)
{


    int num = 0;
Y: https3.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php?action=boardUpdate&board=" + macAdd + "&gpio=" + key + "&state=" + value);
    String outputsState = httpGETRequest3();
    if (outputsState.length() < 10)
    {
        num++;
        if (num < 3)
        {
            Serial.println("Erron");
            goto Y;
        }
    }

}

void SendDelay(String key, String value)
{
    int num = 0;
Y: https3.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php?action=boardUpdateDelay&board=" + macAdd + "&gpio=" + key + "&Delay=" + value);
    String outputsState = httpGETRequest3();
    if (outputsState.length() < 10)
    {
        num++;
        if (num < 3)
        {
            Serial.println("Erron");
            goto Y;
        }
    }

}

void CheckAlarm()
{
    try
    {

        Second += 2;
        if (Second >= 60)
        {
            Second = 0; Min++;
            if (!IsHour)
            {
                tmGet = millis();
                IsHour = true;
            }
        }
        if (Min >= 60)
        {
            Min = 0; Hour++;
        }
        // Serial.println(String(Hour)+":"+String(Min)+":"+String(Second));
        if (Hour >= 24)Hour = 0;
        if (Second > 10 && IsOn)IsOn = false;
        if (Second > 10 || IsOn)return;
        // Serial.println(String(Hour) + ":" + String(Min) + ":" + String(Second));
        bool IsAction = false;
        for (int i; i < 4; i++)
        {
            if (arGPIO[i] != -1)
            {
                if (Day == "Monday" && arDay[0] == "True") IsAction = true;
                else if (Day == "Tuesday" && arDay[1] == "True")IsAction = true;
                else if (Day == "Wednesday" && arDay[2] == "True")IsAction = true;
                else if (Day == "Thursday" && arDay[3] == "True")IsAction = true;
                else if (Day == "Friday" && arDay[4] == "True")IsAction = true;
                else if (Day == "Saturday" && arDay[5] == "True")IsAction = true;
                else if (Day == "Sunday" && arDay[6] == "True")IsAction = true;
                if (IsAction)
                {
                    // Serial.println("Days");
                    if (arOn[i] == "")continue;
                    split(arOn[i], '_');
                    for (int j = 0; j < countSP; j++)
                    {
                        String hON = arSP[j];
                        if (hON == "")continue;
                        split2(hON, '-');

                        if (countSP2 > 0)
                        {
                            String HourON = arSP2[0];
                            String MinOn = arSP2[1];

                            if (HourON.toInt() == Hour && MinOn.toInt() == Min)
                            {

                                if (arGPIO[i] > 3)continue;
                                IsOn = true;
                                Serial.println("On");
                                digitalWrite(IO[arGPIO[i] - 1], HIGH);
                                SendReq(String(arGPIO[i]), "1");

                            }
                        }
                        else
                            continue;
                    }
                    if (arOff[i] == "")continue;
                    split(arOff[i], '_');
                    for (int j = 0; j < countSP; j++)
                    {
                        String hOFF = arSP[j];
                        if (hOFF == "")break;
                        split2(hOFF, '-');
                        if (countSP2 > 0)
                        {
                            String HourOff = arSP2[0];
                            String MinOff = arSP2[1];
                            if (HourOff.toInt() == Hour && MinOff.toInt() == Min)
                            {
                                if (arGPIO[i] > 3)continue;  Serial.println("Off");
                                digitalWrite(IO[arGPIO[i] - 1], LOW); IsOn = true;
                                SendReq(String(arGPIO[i]), "0");

                            }
                        }
                        else continue;
                    }
                }
            }


        }

    }
    catch (...)
    {

    }


}

bool IsDelays = false;
long tmDelays;
int delays2 = 0, delayOld = 0;

void GetDelay() {
    try
    {


        https2.begin(*client, getDelay + macAdd);
        outputsState = httpGETRequest2();
        JSONVar myObject = JSON.parse(outputsState);
        if (JSON.typeof(myObject) == "undefined" || outputsState == "null") {
            Serial.println("Parsing input failed delay!");
        }
        else
        {

            keys = myObject.keys();
            for (int i = 0; i < keys.length(); i++) {
                int pin = atoi(keys[i]);

                if (pin > 4 || pin < 1)continue;
                JSONVar value = myObject[keys[i]];
                int val = atoi(value);
                if (val > 0)
                {

                    if (valDelay[pin - 1] != val)
                    {
                        tmDelay[pin - 1] = millis();
                        valDelay[pin - 1] = val;

                    }
                }

                if (valDelay[pin - 1] > 0 && boolDelay[pin - 1] == false)
                {
                    boolDelay[pin - 1] = true;

                }

            }

            for (int i = 0; i < 4; i++)
            {

                if (boolDelay[i] == true)
                {

                    if (millis() - tmDelay[i] > valDelay[i] * 60000)
                    {

                        Serial.println("OFF");

                        boolDelay[i] = false;
                        valDelay[i] = 0;
                        int value = 0;
                        digitalWrite(IO[i], value);
                        preferences.putUInt(IO[i] + "", value);
                        int num = 0;
                        int gpio = i + 1;
                        SendDelay(String(gpio), String(value));
                        SendReq(String(gpio), String(value));


                    }

                }
            }
        }
    }
    catch (...)
    {
        Serial.println("Error Delay");
    }


}

void GetAlarm() {
    try
    {
        https2.begin(*client, getAlarm + macAdd);
        outputsState = httpGETRequest2();
        // Serial.println(outputsState);
        JSONVar myObject = JSON.parse(outputsState);
        if (JSON.typeof(myObject) == "undefined" || outputsState == "null") {
            Serial.println("Parsing input failed!");
        }
        else
        {
            // myObject.keys() can be used to get an array of all the keys in the object
            keys = myObject.keys();
            arGPIO[0] = -1; arGPIO[1] = -1; arGPIO[2] = -1;
            for (int i = 0; i < keys.length(); i++) {
                JSONVar value = myObject[keys[i]];
                String Alarm = value;
                if (Alarm != "")
                {
                    split(Alarm, '\n');
                    String spDay = arSP[0];
                    arOn[i] = arSP[1];
                    arOff[i] = arSP[2];

                    if (countSP >= 3)
                    {
                        arGPIO[i] = atoi(keys[i]);

                        //  Serial.println("GPIO " + String(arGPIO[i]));
                        split(spDay, '_');

                        // Serial.println("Day :");
                        for (int i = 0; i < countSP; i++)
                        {
                            arDay[i] = arSP[i];
                            //    Serial.print(arDay[i] + "-");
                        }

                        //     Serial.println();
                    }
                    //  Serial.println("ON " + arOn[i]);
                    //   Serial.println("OFF " + arOff[i]);
                }
                // preferences.putUInt(IO[atoi(keys[i]) - 1] + "", val);

                //  digitalWrite(IO[atoi(keys[i]) - 1], atoi(value));
            }
        }
        int numEr = 0;
    X: if (numEr < 100)
        https2.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php?action=getHour");
    outputsState = httpGETRequest2();
    if (outputsState.length() < 5)return;
    split(outputsState, ' ');
    Day = arSP[0];
    String Hours = arSP[1];
    if (Hours == "")
    {
        numEr++; goto X;
    }
    split(Hours, ':');
    if (countSP < 2) {
        numEr++; goto X;
    }
    String sHour = arSP[0];
    String sMin = arSP[1];
    String sSencord = arSP[2];
    Hour = sHour.toInt();
    Min = sMin.toInt();
    Second = sSencord.toInt();

    }
    catch (...)
    {

    }

}

bool IsReset = false;
long tmReset = 0;

void configModeCallback(WiFiManager* myWiFiManager) {
    timer1_detachInterrupt();
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());

    Serial.println(myWiFiManager->getConfigPortalSSID());
    for (int j = 0; j < 20; j++)
    {

        digitalWrite(Blue, LOW);
        delay(50);
        digitalWrite(Blue, HIGH);
        delay(50);
    }

}

void saveConfigCallback() {
    Serial.println("Should save config");
}

long tm1, tm2, tm3, tm4, tm;

long tm12, tm22, tm32;

int numPress = 0;

void Default()
{

    wifiManager.resetSettings();
    ESP.eraseConfig();
    delay(2000);
    ESP.reset();  preferences.putString("default", "");
}

bool IsDefault = false;



WiFiManagerParameter custom_field; // global param ( for non blocking w params )


void Change()
{
    if (valGPIO[numPress - 1] > 1)valGPIO[numPress - 1] = 1;
    if (valGPIO[numPress - 1] == 0)valGPIO[numPress - 1] = 1;
    else valGPIO[numPress - 1] = 0;
    digitalWrite(IO[numPress - 1], valGPIO[numPress - 1]);

}

void ICACHE_RAM_ATTR Button1() {
    try
    {

        if (IsAttach)return;

        if (millis() - tm > delayPress)//150
        {

            if (numPress == 0)
            {
                tm = millis();
                numPress = 1; IsAttach = true;
                Change();


            }
        }
    }
    catch (...)
    {

    }
}

void ICACHE_RAM_ATTR Button2() {
    try
    {

        if (IsAttach)return;

        if (millis() - tm > delayPress)
        {

            if (numPress == 0)
            {
              Is2 = true;
                tm = millis();
                numPress = 2;  IsAttach = true;
                Change();

            }

        }
    }
    catch (...)
    {

    }



}

void ICACHE_RAM_ATTR Button3() {
    try
    {

        if (IsAttach)return;

        if (millis() - tm > delayPress)
        {

            if (numPress == 0)
            {
                tm = millis();
                Is3 = true;
                numPress = 3;
                IsAttach = true;
                Change();
            }

        }
    }
    catch (...)
    {

    }
}




void ICACHE_RAM_ATTR Button4() {

    try
    {

        if (IsAttach)return;

        if (millis() - tm > delayPress)
        {
            if (numPress == 0)
            {
                tm = millis();
                Is4 = true;
                numPress = 4;
                IsAttach = true;
                Change();
            }
        }
    }
    catch (...)
    {

    }
}

bool IsSend = false;

void SendIO()
{
    try
    {
        https2.end();
        SendReq(String(numPress), String(valGPIO[numPress - 1]));
        if (valGPIO[numPress - 1] == 0 && valDelay[numPress - 1] != 0)
        {
            SendDelay(String(numPress), "0");
        }

        numPress = 0;
        IsAttach = false;
    }
    catch (...)
    {

    }

}

void setup() {
    preferences.begin("myfile", false);

    pinMode(btn1, INPUT_PULLUP);
    pinMode(btn2, INPUT_PULLUP);
    pinMode(btn3, INPUT_PULLUP);
    pinMode(btn4, INPUT_PULLUP);

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    pinMode(GPIO1, OUTPUT);
    pinMode(GPIO2, OUTPUT);
    pinMode(GPIO3, OUTPUT);
    pinMode(GPIO4, OUTPUT);
    pinMode(Blue, OUTPUT);
    digitalWrite(Blue, HIGH);
    for (int i = 0; i < 4; i++)
    {

        valGPIO[i] = preferences.getUInt(IO[i] + "", 0);
        digitalWrite(IO[i], valGPIO[i]);
    }
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    std::vector<const char*> menu = { "wifi", "exit" }; //,"sep","restart"
    wifiManager.setMenu(menu);
    // set dark theme
    wifiManager.setClass("invert");
    wifiManager.setConnectTimeout(200);
    wifiManager.autoConnect("BeeHome");
    client->setInsecure();

    // Your IP address with path or Domain name with URL path



    if (WiFi.status() == WL_CONNECTED)
    {
        attachInterrupt(digitalPinToInterrupt(btn1), Button1, FALLING);
        attachInterrupt(digitalPinToInterrupt(btn2), Button2, FALLING);
        attachInterrupt(digitalPinToInterrupt(btn3), Button3, FALLING);
        attachInterrupt(digitalPinToInterrupt(btn4), Button4, FALLING);
        Serial.println(WiFi.RSSI());
        macAdd = WiFi.macAddress();
        macAdd.replace(":", "_");
        https2.begin(*client, serverName2 + macAdd);
        // Send HTTP POST request
        int httpResponseCode2 = https2.GET();
        String sGet = https2.getString();
        Serial.println(sGet);
        if (sGet == "null") {
            String httpRequestData = "action=insert_mac & board=" + macAdd + "&type=O3";
            Serial.println(httpRequestData);
            https2.end();
            https2.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php");//serverName + macAdd);
            https2.addHeader("Content-Type", "application/x-www-form-urlencoded");

            int httpResponseCode = https2.POST(httpRequestData);
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                preferences.putString("default", "0");
                https2.end();
            }
            else {
                Serial.print("Error code1: ");
                Serial.println(httpResponseCode);
            }
        }
        else   preferences.putString("default", "0");
        GetAlarm();
        https.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php?action=connect&board=" + macAdd);
        int httpResponseCode = https.GET();
        sGet = https.getString();
        if (sGet.length() < 4)
        {
            httpResponseCode = https.GET();
            https.begin(*client, "https://giacongpcb.vn/esp-outputs-action.php?action=connect&board=" + macAdd);
        }
        Serial.println(sGet);
        https.begin(*client, serverName + macAdd);
    }

    tmGet = millis();
    tmCheck = millis();
    tmReset = millis();
    tm1 = millis();
    tm2 = millis();
    tm3 = millis();
    tm4 = millis(); tm = millis();
}

void Error()
{

Y:  https2.begin(*client, serverName2 + macAdd);
    int httpResponseCode2 = https2.GET();
    Serial.println(https2.getString());
    if (https2.getString() == "null") {
        Serial.println("Reset");
        wifiManager.resetSettings();
        ESP.eraseConfig();
        delay(2000);
        ESP.reset();  preferences.putString("default", "");
    }
    else
    {
        delay(100);
        numErr++;
        if (numErr > 5000)
        {
            delay(1000);
            ESP.reset();
        }
    }
    https2.end();
    goto Y;
}

bool IsEnAttach = false;
long tmAttach = 0;

void loop() {

    try
    {
        if (Is2 && Is3)
        {

            if (millis() - tm2 > 3000 && millis() - tm3 > 3000)
            {
                if (digitalRead(btn2) == LOW && digitalRead(btn3) == LOW)
                    Default();
            }


        }
        if (numPress > 0)
        {
            SendIO();
        }
        else if (millis() - previousMillis > interval)//100
        {
            if (WiFi.status() == WL_CONNECTED) {
                digitalWrite(Blue, LOW);
                outputsState = httpGETRequest(serverName);
                Serial.println(outputsState);
                JSONVar myObject = JSON.parse(outputsState);
                if (JSON.typeof(myObject) == "undefined" || outputsState == "null") {
                    Serial.println("Parsing input failed!");
                    Error();
                }
                numErr = 0;
                keys = myObject.keys();
                for (int i = 0; i < keys.length(); i++) {
                    int pin = atoi(keys[i]);
                    if (pin > 4 || pin < 1)continue;
                    JSONVar value = myObject[keys[i]];
                    int val = atoi(value);
                    if (val > 1)val = 1;
                    if (numPress == 0)
                    {
                        valGPIO[pin - 1] = val;
                        digitalWrite(IO[pin - 1], val);
                    }
                    else
                    {
                        SendIO();
                        return;
                    }
                }
                previousMillis = millis();
            }
            else {
                digitalWrite(Blue, HIGH);
                Serial.println("WiFi Disconnected");
            }

        }
        else
        {
            if (millis() - tmGet > 120000)
            {
                GetAlarm();
                IsHour = false;
            }
            else if (millis() - tmCheck > 2000)
            {
                tmCheck = millis();
                CheckAlarm();
                GetDelay();
                /* for (int i = 0; i < 4; i++)
                 {
                  // preferences.putUInt(IO[i] + "",  valGPIO[i]);
                 }*/
            }
        }

    }
    catch (...)
    {

    }

}

String httpGETRequest(const char* serverName) {


    // Send HTTP POST request
    int httpResponseCode = https.GET();

    String payload = "{}";

    if (httpResponseCode > 0) {
        //  Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        payload = https.getString();
    }
    else {
        Serial.print("Error code2: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    // https.end();

    return payload;
}
