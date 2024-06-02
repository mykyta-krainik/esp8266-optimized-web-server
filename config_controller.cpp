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
  String path = "/static/config.html.gz";

  if (Files::get_instance().file_exists(path)) {
    File html_base = Files::get_instance().get_file(path);
    server.streamFile(html_base, "text/html");
  } else {
    server.sendHeader("Location", "/", true);
    server.send(307, "text/plain", "Configuration page not found");
    server.client().stop();
  }
}

void ConfigController::handle_get_wifi() {
  String wifi_configurations = wifi_config.get_all_networks();

  server.send(200, "application/json", wifi_configurations);
  server.client().stop();
}

void ConfigController::handle_patch_wifi() {
  middlewares->execute(server, *context);

  const char* ssid = context->jsonDoc["ssid"];
  const char* password = context->jsonDoc["password"];

  if (wifi_config.update_network(ssid, password)) {
    server.send(200, "text/plain", "Wi-Fi configuration updated");
    server.client().stop();

    return;
  }

  server.send(500, "text/plain", "Failed to update Wi-Fi configuration");
  server.client().stop();
}

void ConfigController::handle_delete_wifi() {
  String ssid = server.arg("ssid");

  if (wifi_config.remove_network(ssid.c_str())) {
    server.send(200, "text/plain", "Wi-Fi network removed");
    server.client().stop();

    return;
  }

  server.send(404, "text/plain", "Network not found");
  server.client().stop();
}
