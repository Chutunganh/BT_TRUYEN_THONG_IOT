/************ IMPORT ************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/************ CONFIG ************/
const char* ssid = "Redmi K40 Pro";
const char* password = "88888888";

const char* mqtt_server = "192.168.0.245";   // IP Home Assistant
const int   mqtt_port   = 1883;

const char* mqtt_user = "chutunganh";
const char* mqtt_pass = "12345678";

const char* student_id = "10123004";

/************ MQTT TOPICS ************/
const char* topic_cmd   = "iot/lab2/10123004/cmd";
const char* topic_state = "iot/lab2/10123004/state";

/************ OBJECTS ************/
WiFiClient espClient;
PubSubClient client(espClient);

/************ LED ************/
/*
 NodeMCU:
 - GPIO2 = D4 = LED onboard (active LOW)
*/
#define LED_PIN 2
bool ledState = false;

/* ================== WIFI ================== */
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/* ================== MQTT CALLBACK ================== */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "ON") {
    ledState = true;
    digitalWrite(LED_PIN, LOW);   // ESP8266 LED active LOW
  }
  else if (message == "OFF") {
    ledState = false;
    digitalWrite(LED_PIN, HIGH);
  }

  // Publish trạng thái LED
  client.publish(topic_state, ledState ? "ON" : "OFF", true);
}

/* ================== MQTT RECONNECT ================== */
void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266-Lab2-";
    clientId += student_id;

    Serial.print("Connecting to MQTT... ");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");

      client.subscribe(topic_cmd);
      Serial.print("Subscribed to ");
      Serial.println(topic_cmd);

      // Gửi trạng thái ban đầu
      client.publish(topic_state, ledState ? "ON" : "OFF", true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2s");
      delay(2000);
    }
  }
}

/* ================== SETUP ================== */
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // LED OFF

  Serial.begin(115200);
  delay(1000);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(60);
}

/* ================== LOOP ================== */
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
