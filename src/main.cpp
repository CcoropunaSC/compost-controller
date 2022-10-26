#include <Arduino.h>
#include "Colors.h"
#include "IoTicosSplitter.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

String dId = "1212";
String webhook_pass = "SA7sxAKDVR";
String webhook_endpoint = "http://192.168.0.109:3001/api/getdevicecredentials";
const char *mqtt_server = "192.168.0.109";

// PINS
#define led 2

// WiFi
const char *wifi_ssid = "EpicSpace";
const char *wifi_password = "SECuRE77&90Z_";

void check_mqtt_connection();
bool reconnect();
void process_sensors();
void clear();

// Global Vars
WiFiClient espclient;
PubSubClient client(espclient);
// IoTicosSplitter splitter;
long lastReconnectAttemp = 0;
// long varsLastSend[20];
// String last_received_msg = "";
// String last_received_topic = "";
// int prev_temp = 0;
int sensorHum = 34;

DynamicJsonDocument mqtt_data_doc(1048);

void setup()
{
    Serial.begin(921600);
    pinMode(led, OUTPUT);
    pinMode(sensorHum, INPUT);
    clear();

    Serial.println("INSPIRATION FOR!!");

    WiFi.begin(wifi_ssid, wifi_password);

    int counter = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        counter++;

        if (counter > 10)
        {
            Serial.print("  ⤵" + fontReset);
            Serial.print(Red + "\n\n         Ups WiFi Connection Failed :( ");
            Serial.println(" -> Restarting..." + fontReset);
            delay(2000);
            ESP.restart();
        }
    }

    Serial.print("  ⤵" + fontReset);

    // Printing local ip
    Serial.println(boldGreen + "\n\n         WiFi Connection -> SUCCESS :)" + fontReset);
    Serial.print("\n         Local IP -> ");
    Serial.print(boldBlue);
    Serial.print(WiFi.localIP());
    Serial.println(fontReset);
}

void loop()
{
    check_mqtt_connection();
    process_sensors();
    delay(3000);
}

bool reconnect()
{
    // if (!get_mqtt_credentials())
    // {
    //     Serial.println(boldRed + "\n\n      Error getting mqtt credentials :( \n\n RESTARTING IN 10 SECONDS");
    //     Serial.println(fontReset);
    //     delay(10000);
    //     ESP.restart();
    // }

    // Setting up Mqtt Server
    client.setServer(mqtt_server, 1883);

    Serial.print(underlinePurple + "\n\n\nTrying MQTT Connection" + fontReset + Purple + "  ⤵");

    String str_client_id = "D" + dId + "_" + random(1, 9999);
    // const char *username = mqtt_data_doc["username"];
    // const char *password = mqtt_data_doc["password"];
    String str_topic = "P1/T1";

    if (client.connect(str_client_id.c_str()))
    {
        Serial.print(boldGreen + "\n\n         Mqtt Client Connected :) " + fontReset);
        delay(2000);

        client.subscribe(str_topic.c_str());
        return true;
    }
    else
    {
        Serial.print(boldRed + "\n\n         Mqtt Client Connection Failed :( " + fontReset);
        return false;
    }
}

void check_mqtt_connection()
{

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(Red + "\n\n         Ups WiFi Connection Failed :( ");
        Serial.println(" -> Restarting..." + fontReset);
        delay(15000);
        ESP.restart();
    }

    if (!client.connected())
    {

        long now = millis();

        if (now - lastReconnectAttemp > 5000)
        {
            lastReconnectAttemp = millis();
            if (reconnect())
            {
                lastReconnectAttemp = 0;
            }
        }
    }
    else
    {
        client.loop();
    }
}

int prev_temp = 0;
int prev_hum = 0;

void process_sensors()
{
    // int temp = random(1, 100);
    long humData = analogRead(sensorHum);
    Serial.println(humData);
    int humPresition = humData - 595;
    int humValue = int((humPresition * 100) / 3500); 
    Serial.println(humValue);

    char buf [100];
    snprintf(buf, sizeof(buf), "{\"value\":%d}", humValue);
    // String toSend = "{'value':'"+temp.to_string()+"'}";

    // String toSend = "";

    // mqtt_data_doc["pila1"][0]["temperatura"]["value"] = temp;
    // serializeJson(mqtt_data_doc["pila1"][0]["temperatura"]["value"], toSend);

    String str_topic = "P1/T1";

    client.publish(str_topic.c_str(), buf);
}

void clear()
{
    Serial.write(27);    // ESC command
    Serial.print("[2J"); // clear screen command
    Serial.write(27);
    Serial.print("[H"); // cursor to home command
}