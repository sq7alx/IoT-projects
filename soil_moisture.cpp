#include <Arduino.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

const char* ssid = "UPC8059263";
const char* wifi_password = "ceZ4vdja2Xwm";

const char* mqtt_server = "192.168.0.164";
const char* humidity_topic = "home/kwiatek1/wilgotnosc";
const char* mqtt_username = "serwer";
const char* mqtt_password = "root";
const char* clientID = "kwiatek1";

WiFiClient wifiClient;

void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);
  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

 int dryValue = 786; //dla esp-12e
 int wetValue = 390; //dla esp-12e
//int dryValue = 780; //dla esp8266mod
//int wetValue = 396; //dla esp8266mod

void setup() {
  Serial.begin(115200);
}
 void loop() {
  connect_MQTT();
  PubSubClient client(wifiClient); 
  client.setServer(mqtt_server,1883);
  Serial.setTimeout(2000);
  int sensorValue = analogRead(A0);
  Serial.print(sensorValue);
  int Moisture = map(sensorValue, dryValue, wetValue, 0, 100);
  Serial.print("Wilgotność: ");
  Serial.print(Moisture);
  Serial.println("%");
  String hs="Hum: "+String((float)Moisture)+" % ";
  client.connect(clientID);
  if (client.publish(humidity_topic, String(hs).c_str())) {
    Serial.println("Humidity sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(humidity_topic, String(hs).c_str());
  }
  client.disconnect();
  delay(1000*60);       // print new values every 1 Minute
 }