#ifndef VIEW_CONTROLLER_H
#define VIEW_CONTROLLER_H

#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <vector>
#include "files_model.h"

class ViewController {
public:
  ViewController(ESP8266WebServer& server);
  void setup_routes();
  bool is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types);

private:
  ESP8266WebServer& server;

  void handle_get_view();
  void handle_post_view();
  void handle_delete_view();

  void handle_get_css();
  void handle_post_css();
  void handle_delete_css();

  void handle_get_js();
  void handle_post_js();
  void handle_delete_js();

  void handle_upload_file(const String& path_to_save, std::vector<const char*> valid_types);
};

#endif
