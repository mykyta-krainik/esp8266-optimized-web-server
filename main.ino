#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "page_controller.h"

const char* ssid = "_";
const char* password = "_";

ESP8266WebServer server(80);
PageController pageController(server);

bool connect_to_wifi() {
  WiFi.begin(ssid, password);

  Serial.println("Connecting to Wi-Fi.");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");

  return true;
}

void setup() {
  Serial.begin(115200);

  const bool result = connect_to_wifi();

  if (!result) {
    return;
  }

  if (!MDNS.begin("esp8266-web-server")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  server.begin();
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pageController.setupRoutes();

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
  MDNS.update();
}
