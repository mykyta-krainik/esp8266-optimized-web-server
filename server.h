#ifndef MICRO_SERVER_H
#define MICRO_SERVER_H

#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "files_model.h"
#include "view_controller.h"
#include "base_controller.h"
#include "config_controller.h"
#include "sensors-controller.h"
#include "middleware_chain.h"
#include "json_middleware.h"
#include "middleware_context.h"

class MicroServer {
public:
  MicroServer(uint16_t port = 80);
  ~MicroServer();

  void on_setup();
  void on_loop();

  void setup_routes();
  void register_mDNS(const char* url);
  bool connect_to_wifi();
  bool init_fs();

private:
  AsyncWebServer* server;
  BaseController* base_controller;
  ViewController* view_controller;
  ConfigController* config_controller;
  SensorsController* sensors_controller;
};

#endif
