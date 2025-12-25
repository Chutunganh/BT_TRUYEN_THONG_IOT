#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

const char* WIFI_SSID = "Redmi K40 Pro";
const char* WIFI_PASS = "88888888";

IPAddress serverIP(192, 168, 0, 245); // IP Node.js CoAP server
const int serverPort = 5683;
const char* resource = "sensor/temp";

WiFiUDP udp;
Coap coap(udp);

// Callback nhận response từ server
void callback(CoapPacket &packet, IPAddress ip, int port) {
  Serial.print("Response from ");
  Serial.print(ip);
  Serial.print(":");
  Serial.println(port);

  Serial.print("MID: ");
  Serial.println(packet.messageid);

  Serial.print("Payload: ");
  for (int i = 0; i < packet.payloadlen; i++) {
    Serial.print((char)packet.payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  coap.response(callback);
  coap.start();

  Serial.println("CoAP client started");
}

void loop() {
  coap.loop(); // rất quan trọng để nhận response

  static unsigned long lastGet = 0;
  if (millis() - lastGet >= 2000) {
    lastGet = millis();

    Serial.println("Sending CoAP GET...");
    coap.get(serverIP, serverPort, resource);
  }
}
