#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <vector>

#include "files_model.h"

class BaseController {
public:
  BaseController(ESP8266WebServer& server);
  void setup_routes();

  bool is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types);

private:
  ESP8266WebServer& server;

  void handle_get_upload_page();
  void handle_post_upload_page();
  void handle_get_files_list();
  void handle_not_found();

  void handle_upload_file(const String& path_to_save, std::vector<const char*> valid_types);
};

#endif
