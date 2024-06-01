#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Arduino.h>

#include "files_model.h"

class BaseController {
public:
  BaseController(ESP8266WebServer& server);
  void setup_routes();

private:
  ESP8266WebServer& server;

  void handleGetUploadPage();
  void handleGetFilesList();
  void handleNotFound();
};

#endif
