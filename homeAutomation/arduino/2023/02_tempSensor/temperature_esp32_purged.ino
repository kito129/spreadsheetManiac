/**************************************************************************************************************************************************
* File name     : ESP32_HA_Discovery.c
* Compiler      : 
* Autor         : kito   
* Created       : 2023/04/01
* Modified      : 
* Last modified : 2023/04/01
*
*
* Description   : 
*
* Other info    : Discovery MQTT Device Example
**************************************************************************************************************************************************/


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------Include Files----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// for temperature sensor DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------Local definitions------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#define PERIOD_MILLSEC_1000    1000
#define PERIOD_MILLSEC_500     500
#define PERIOD_MILLSEC_250     250


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------I/O Definitions--------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
const int DS18B20_1 = 5; 
const int DS18B20_2 = 23;

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ Configuration --------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
const char*         g_ssid = "";
const char*         g_password = "";
const char*         g_mqtt_server = "";                           // MQTT Server IP, same of Home Assistant
const char*         g_mqttUser = "";                                 // MQTT Server User Name
const char*         g_mqttPsw = "";                                   // MQTT Server password
int                 g_mqttPort = ;                                        // MQTT Server Port

// Variable used for MQTT Discovery
const char*         g_deviceModel = "ESP32Device";                            // Hardware Model
const char*         g_swVersion = "1.0";                                      // Firmware Version
const char*         g_manufacturer = "kito";                               // Manufacturer Name
String              g_deviceName = "temperatureSensor";                            // Device Name
String              g_mqttStatusTopic = "homeassistant/sensor/esp32iotsensor/" + g_deviceName;     // MQTT Topic
            

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------Public variables-------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
WiFiClient          g_WiFiClient;
PubSubClient        g_mqttPubSub(g_WiFiClient);
unsigned long       g_Time = 0;
int                 g_count = 0;
int                 g_input_Door; 
int                 g_old_input_Door = 0;
String              g_strDoorStatus;
int                 g_mqttCounterConn = 0;
float               g_Humidity = 0.0;
float               g_Temperature_1 = 0.0;
float               g_Temperature_2 = 0.0;
bool                g_InitSystem = true;
String              g_UniqueId;
bool                g_SendMqttData = false;

// temperature sensor
OneWire oneWire_1(DS18B20_1); // connection to oneWire
OneWire oneWire_2(DS18B20_2); // connection to oneWire
DallasTemperature tempSensor1(&oneWire_1); // declare of sensor object
DallasTemperature tempSensor2(&oneWire_2); // declare of sensor object
             
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ SETUP ----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void setup() 
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Input output Pin definitions
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pinMode(DS18B20_1, INPUT);
    pinMode(DS18B20_2, INPUT);

    tempSensor1.begin();
    tempSensor2.begin();


    Serial.begin(115200);
    delay(500);

    Serial.println("");
    Serial.println("");
    Serial.println("----------------------------------------------");
    Serial.print("MODEL: ");
    Serial.println(g_deviceModel);
    Serial.print("DEVICE: ");
    Serial.println(g_deviceName);
    Serial.print("SW Rev: ");
    Serial.println(g_swVersion);
    Serial.println("----------------------------------------------");
   
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Wifi Init
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    setup_wifi();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MQTT Init
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    g_mqttPubSub.setServer(g_mqtt_server, g_mqttPort);
    g_mqttPubSub.setCallback(MqttReceiverCallback);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ LOOP -----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void loop() 
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MQTT Connection
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(WiFi.status() == WL_CONNECTED)
    {
        if(!g_mqttPubSub.connected())
            MqttReconnect();
        else
            g_mqttPubSub.loop();
    }

    if(g_InitSystem){
        delay(100);
        g_InitSystem = false;
        Serial.println("INIT SYSTEM...");
        MqttHomeAssistantDiscovery();     // Send Discovery Data
    }

    if(millis() - g_Time > PERIOD_MILLSEC_500){  // Every 500 [msec]
        g_Time = millis();
        
        if(g_count++ == 20 || g_SendMqttData) // Every 10 [sec] or if input status changed
        {
            g_count = 0;

            ////////////////////////////////////////////////////////////////
            // TEMPERATURE
            ////////////////////////////////////////////////////////////////
            tempSensor1.requestTemperatures();
            tempSensor2.requestTemperatures();
            g_Temperature_1 = tempSensor1.getTempCByIndex(0);
            g_Temperature_2 = tempSensor2.getTempCByIndex(0);

            Serial.print("Temperature 1: ");
            Serial.print(g_Temperature_1);
            Serial.println(" °C");

            Serial.print("Temperature 2: ");
            Serial.print(g_Temperature_2);
            Serial.println(" °C");
            
            g_SendMqttData = true;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // SEND MQTT DATA
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
        if(g_SendMqttData == true){
            Serial.println(g_Time);
            StaticJsonDocument<200> payload;  
            payload["temp_1"] = g_Temperature_1;
            payload["temp_2"] = g_Temperature_2;

            String strPayload;
            serializeJson(payload, strPayload);

            if(g_mqttPubSub.connected())
            {
                g_mqttPubSub.publish(g_mqttStatusTopic.c_str(), strPayload.c_str()); 
                Serial.println("MQTT: Send Data!!!");
                Serial.println(strPayload.c_str());
                Serial.println(" ");
                Serial.println(" ");
                g_SendMqttData = false;
            }
        }
    }
}


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ Public Functions -----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void setup_wifi() 
{
    int counter = 0;
    byte mac[6];
    delay(10);
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(g_ssid);

    WiFi.begin(g_ssid, g_password);

    WiFi.macAddress(mac);
    g_UniqueId =  String(mac[0],HEX) +String(mac[1],HEX) +String(mac[2],HEX) +String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX);

    Serial.print("Unique ID: ");
    Serial.println(g_UniqueId);    
   
    while(WiFi.status() != WL_CONNECTED && counter++ < 8) 
    {
        delay(50000);
        Serial.print(".");
    }
    Serial.println("");

    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else
    {
        Serial.println("WiFi NOT connected!!!");
    }
}

void MqttReconnect() 
{
    // Loop until we're reconnected
    while (!g_mqttPubSub.connected()  && (g_mqttCounterConn++ < 4))
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (g_mqttPubSub.connect(g_deviceName.c_str(), g_mqttUser, g_mqttPsw)) 
        {
            Serial.println("connected");
            // Subscribe
            g_mqttPubSub.subscribe("homeassistant/status");
            delay(5000);
        } else 
        {
            Serial.print("failed, rc=");
            Serial.print(g_mqttPubSub.state());
            Serial.println(" try again in 1 seconds");
            delay(1000);
        }
    }  
    g_mqttCounterConn = 0;
}

void MqttHomeAssistantDiscovery()
{
    String discoveryTopic;
    String payload;
    String strPayload;
    if(g_mqttPubSub.connected())
    {
        Serial.println("SEND HOME ASSISTANT DISCOVERY!!!");
        StaticJsonDocument<600> payload;
        JsonObject device;
        JsonArray identifiers;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Temperature
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/sensor/esp32iotsensor/" + g_deviceName + "_temp" + "/config";
        
        payload["name"] = g_deviceName + ".temp";
        payload["uniq_id"] = g_UniqueId + "_temp";
        payload["stat_t"] = g_mqttStatusTopic;
        payload["dev_cla"] = "temperature";
        payload["val_tpl"] = "{{ value_json.temp | is_defined }}";
        payload["unit_of_meas"] = "°C";
        device = payload.createNestedObject("device");
        device["name"] = g_deviceName;
        device["model"] = g_deviceModel;
        device["sw_version"] = g_swVersion;
        device["manufacturer"] = g_manufacturer;
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);

        serializeJsonPretty(payload, Serial);
        Serial.println(" ");
        serializeJson(payload, strPayload);

        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
    }
}

void MqttReceiverCallback(char* topic, byte* inFrame, unsigned int length) 
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    byte state = 0;
    String messageTemp;
    
    for (int i = 0; i < length; i++) 
    {
        Serial.print((char)inFrame[i]);
        messageTemp += (char)inFrame[i];
    }
    Serial.println();
  
    if(String(topic) == String("homeassistant/status")) 
    {
        if(messageTemp == "online")
            MqttHomeAssistantDiscovery();
    }
}