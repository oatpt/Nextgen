#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTTYPE DHT11

//=================================================================================================
WiFiClient   espClient;
PubSubClient client(espClient);             
//=================================================================================================
const char* ssid = "WIFINAME";               
const char* password = "PASSWORD";         
//=================================================================================================
const char* mqtt_broker = "broker.hivemq.com";   
const int   mqtt_port = 1883;               
//=================================================================================================
const int sensor_rain = 34; 
const int sensor_dht11 = 15; 
const int relay_fan = 33; 
const int relay_ligth = 32; 
long long time_start;

DHT dht(sensor_dht11, DHTTYPE);

void setup_wifi() {   
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);              
  WiFi.begin(ssid, password);       
  while (WiFi.status() != WL_CONNECTED)     
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {  
  payload[length]='\0';
  if(strcmp(topic,"NextGen_23/smartHome/actuator/light")==0)
  {
    Serial.println((char*)payload);
    if(strcmp((char*)payload,"on")==0)
    {
      digitalWrite(relay_ligth,HIGH);
      Serial.println("high");
    }
    else 
    {
      digitalWrite(relay_ligth,LOW);
      Serial.println("low");
    }
  }
  // your code to control fan
}

void reconnect() {  
  client.setServer(mqtt_broker, mqtt_port);   
  client.setCallback(callback);               
  while (!client.connected()) 
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str()))
      Serial.println("Public emqx mqtt broker connected");
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  pinMode(relay_ligth,OUTPUT);
  digitalWrite(relay_ligth,LOW);
  pinMode(relay_fan,OUTPUT);
  digitalWrite(relay_fan,LOW);
  
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  reconnect(); 
  client.publish("NextGen_23/smartHome/sensor/rain", "false");
  client.subscribe("NextGen_23/smartHome/actuator/#");  
   
}

void loop()
{
  client.loop();
  if (millis() - time_start >= 1000)
  {
    
    double humidity = dht.readHumidity();
    double temperature =  dht.readTemperature();
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.println(temperature, 1);
    //your code
    time_start = millis();
  }
 
}