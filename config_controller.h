#ifndef CONFIG_CONTROLLER_H
#define CONFIG_CONTROLLER_H

#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266WiFiMulti.h>

#include "files_model.h"
#include "middleware_chain.h"
#include "middleware_context.h"
#include "json_middleware.h"
#include "wifi_config_model.h"

class ConfigController {
public:
  ConfigController(ESP8266WebServer& server);
  void setup_routes();

private:
  ESP8266WebServer& server;
  WifiConfig wifi_config;

  MiddlewareChain* middlewares;
  MiddlewareContext* context;

  void handle_get_config_page();
  void handle_get_wifi();
  void handle_patch_wifi();
  void handle_delete_wifi();
};

#endif
