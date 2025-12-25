// IMPORT IF ESP32
// #include <WiFi.h>
// IMPORT IF ESP8266
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>


/* ====== CONFIG ====== */
const char* device_name = "ESP8266";

const char* ssid = "VIETTEL_TUNG ANH";
const char* password = "12162005";

// IP Home Assistant
const char* mqtt_server = "192.168.2.14";
const int mqtt_port = 1883;

const char* mqtt_user = "chutunganh";
const char* mqtt_pass = "12345678";

const char* student_id = "10123004";
const char* mqtt_topic = "iot/lab1/10123004/sensor";

/* ==================== */

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
delay(10);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
}
}

void reconnect() {
while (!client.connected()) {
String clientId = device_name;
clientId += student_id;

if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
Serial.println("MQTT connected");
} else {
Serial.print("MQTT failed, rc=");
Serial.println(client.state());
delay(2000);
}
}
}

void setup() {
Serial.begin(115200);
setup_wifi();
client.setServer(mqtt_server, mqtt_port);
}

void loop() {
if (!client.connected()) {
reconnect();
}
else {
  Serial.println("Connected");
}
client.loop();
StaticJsonDocument<200> doc;
doc["temp"] = random(250, 350) / 10.0;
doc["hum"] = random(400, 800) / 10.0;
char payload[128];
serializeJson(doc, payload);
client.publish(mqtt_topic, payload);

delay(5000);
}