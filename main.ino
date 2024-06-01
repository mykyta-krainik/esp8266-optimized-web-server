#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

#include "server.h"

MicroServer server(80);

void setup() {
  server.on_setup();
}

void loop() {
  server.on_loop();
}
