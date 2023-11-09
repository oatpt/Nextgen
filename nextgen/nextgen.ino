#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

DHT dht;
//=================================================================================================
WiFiClient   espClient;
PubSubClient client(espClient);             //สร้างออปเจ็ค สำหรับเชื่อมต่อ mqtt
//=================================================================================================
const char* ssid = "Oatpt";               //wifi name
const char* password = "23456789";         //wifi password
//=================================================================================================
const char* mqtt_broker = "broker.hivemq.com";   //IP mqtt server
const int   mqtt_port = 1883;               //port mqtt server
//=================================================================================================
const int sensor = 34; 

void setup_wifi() {   //ฟังก์ชั่นเชื่อมต่อwifi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);              //เลือกโหมดรับ wifi
  WiFi.begin(ssid, password);       //เชื่อมต่อ wifi
  while (WiFi.status() != WL_CONNECTED)     //รอจนกว่าจะเชื่อมต่อwifiสำเร็จ
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
      digitalWrite(32,HIGH);
      Serial.println("high");
    }
    else 
    {
      digitalWrite(32,LOW);
      Serial.println("low");
    }
  }
  if(strcmp(topic,"NextGen_23/smartHome/actuator/fan")==0)
  {
    
  }
}

void reconnect() {  //ฟังก์ชั่นเชื่อมต่อmqtt
  client.setServer(mqtt_broker, mqtt_port);   //เชื่อมต่อmqtt
  client.setCallback(callback);               //เลือกฟังก์ชั่นsubscribe
  while (!client.connected()) //รอจนกว่าจะเชื่อมต่อmqttสำเร็จ
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
  pinMode(32,OUTPUT);
  digitalWrite(32,LOW);
  
  Serial.begin(9600);
  dht.setup(15); 
  setup_wifi();
  reconnect(); 
  client.publish("NextGen_23/smartHome/sensor/rain", "false");
  client.subscribe("NextGen_23/smartHome/actuator/#");  
   
}
long long time_start;
void loop()
{
  client.loop();//วนลูปรอsubscribe valu จาก mqtt
  if (millis() - time_start >= 1000)
  {
    delay(dht.getMinimumSamplingPeriod());
    double humidity = dht.getHumidity();
    double temperature = dht.getTemperature();  
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.println(temperature, 1);
    char text[100];
    sprintf(text, "%.1f", 31.6);
    client.publish("NextGen_23/smartHome/sensor/humi", text);
    sprintf(text, "%.1f", 39.4);
    client.publish("NextGen_23/smartHome/sensor/temp", text);
    
    Serial.print("rain : \t\t");
    Serial.println(analogRead(sensor));
    if(analogRead(sensor)<3000)
      client.publish("NextGen_23/smartHome/sensor/rain", "true");
    else
      client.publish("NextGen_23/smartHome/sensor/rain", "false");
    time_start = millis();
  }
 
}
