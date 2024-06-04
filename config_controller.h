#ifndef CONFIG_CONTROLLER_H
#define CONFIG_CONTROLLER_H

#include <ESP8266WiFiMulti.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <functional>

#include "files_model.h"
#include "middleware_chain.h"
#include "middleware_context.h"
#include "json_middleware.h"
#include "wifi_config_model.h"

class ConfigController {
public:
  ConfigController(AsyncWebServer* server);
  ~ConfigController();

  void setup_routes();

private:
  AsyncWebServer* server;
  WifiConfig wifi_config;

  MiddlewareChain* middlewares;
  MiddlewareContext* context;

  void handle_get_config_page(AsyncWebServerRequest* request);
  void handle_get_wifi(AsyncWebServerRequest* request);
  void handle_patch_wifi(AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total);
  void handle_delete_wifi(AsyncWebServerRequest* request);
};

#endif
