#ifndef MICRO_SERVER_H
#define MICRO_SERVER_H

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <Arduino.h>

#include "files_model.h"
#include "view_controller.h"
#include "base_controller.h"

class MicroServer {
public:
  MicroServer(unsigned int port = 80);
  ~MicroServer();

  void on_setup();
  void on_loop();

  void setup_routes();
  void register_mDNS(const char* url);
  bool connect_to_wifi();
  bool init_fs();

private:
  ESP8266WebServer* server;
  BaseController* base_controller;
  ViewController* view_controller;
  // TODO: ConfigController* config_controller;
};

#endif
