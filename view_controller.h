#ifndef VIEW_CONTROLLER_H
#define VIEW_CONTROLLER_H

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <vector>
#include "files_model.h"

class ViewController {
public:
  ViewController(AsyncWebServer* server);
  void setup_routes();
  bool is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types);

private:
  AsyncWebServer* server;

  void handle_get_view(AsyncWebServerRequest* request);
  void handle_delete_view(AsyncWebServerRequest* request);

  void handle_get_css(AsyncWebServerRequest* request);
  void handle_delete_css(AsyncWebServerRequest* request);

  void handle_get_js(AsyncWebServerRequest* request);
  void handle_delete_js(AsyncWebServerRequest* request);

  void handle_upload_file(
    const String& path_to_save,
    std::vector<const char*> valid_types,
    AsyncWebServerRequest *request,
    const String& filename,
    size_t index,
    uint8_t *data,
    size_t len,
    bool final
  );
};

#endif
