#include "config_controller.h"

ConfigController::ConfigController(ESP8266WebServer& serverRef) : server(serverRef) {
  context = new MiddlewareContext();
  middlewares = new MiddlewareChain({ new JsonMiddleware() });
}

void ConfigController::setup_routes() {
  server.on("/config", HTTP_GET, std::bind(&ConfigController::handle_get_config_page, this));
  server.on("/config/wifi", HTTP_GET, std::bind(&ConfigController::handle_get_wifi, this));
  server.on("/config/wifi", HTTP_PATCH, std::bind(&ConfigController::handle_patch_wifi, this));
  server.on("/config/wifi", HTTP_DELETE, std::bind(&ConfigController::handle_delete_wifi, this));
}

void ConfigController::handle_get_config_page() {
  String path = "/static/base.html.gz";

  Serial.println(Files::get_instance().file_exists(path));

  if (Files::get_instance().file_exists(path)) {
    File html_base = Files::get_instance().get_file(path);

    Serial.println("You are here");

    server.streamFile<File>(html_base, "text/html");

    return;
  }

  server.sendHeader("Location", "/", true);
  server.send(307, "text/plain", "Oops, there is no base file for working with upload");
}

void ConfigController::handle_get_wifi() {
  server.send(200, "text/plain", "Ok GET /config");
}

void ConfigController::handle_patch_wifi() {
  middlewares->execute(server, *context);

  const char* abc = context->jsonDoc["abc"];

  Serial.println(abc);

  server.send(200, "text/plain", "Ok GET /config");
}

void ConfigController::handle_delete_wifi() {
  for (int i = 0; i < server.args(); i++) {
    Serial.print("Name: ");
    Serial.println(server.argName(i));

    Serial.print("Value: ");
    Serial.println(server.arg(i));
  }

  server.send(200, "text/plain", "Ok GET /config");
}
