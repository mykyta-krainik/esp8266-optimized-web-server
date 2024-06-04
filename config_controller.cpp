#include "config_controller.h"

ConfigController::ConfigController(AsyncWebServer* serverRef) : server(serverRef) {
  context = new MiddlewareContext();
  middlewares = new MiddlewareChain({ new JsonMiddleware() });
}

ConfigController::~ConfigController() {
  delete context;
  delete middlewares;
}

void ConfigController::setup_routes() {
  server->on("/config", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_config_page(request);
  });

  server->on("/config/wifi", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_wifi(request);
  });

  server->on(
    "/config/wifi",
    HTTP_POST,
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Updated");
    },
    [](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
      Serial.println("");
    },
    [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
      handle_patch_wifi(request, data, len, index, total);
    }
  );

  server->on(
    "/config/wifi",
    HTTP_PATCH,
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Updated");
    },
    [](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
      Serial.println("");
    },
    [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
      handle_patch_wifi(request, data, len, index, total);
    }
  );

  server->on("/config/wifi", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    handle_delete_wifi(request);
  });
}

void ConfigController::handle_get_config_page(AsyncWebServerRequest* request) {
  String path = "/static/config.html.gz";

  if (Files::get_instance().file_exists(path)) {
    File html_base = Files::get_instance().get_file(path);

    AsyncWebServerResponse* response = request->beginResponse(html_base, path, "text/html");

    response->addHeader("Content-Encoding", "gzip");

    request->send(response);
  } else {
    request->redirect("/");
  }
}

void ConfigController::handle_get_wifi(AsyncWebServerRequest* request) {
  String wifi_configurations = wifi_config.get_all_networks();

  request->send(200, "application/json", wifi_configurations);
}

void ConfigController::handle_patch_wifi(AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
  middlewares->execute(request, *context, data);

  const char* ssid = context->jsonDoc["ssid"];
  const char* password = context->jsonDoc["password"];

  if (wifi_config.update_network(ssid, password)) {
    request->send(200, "text/plain", "Wi-Fi configuration updated");

    ESP8266WiFiMulti wifi_multi;

    if (wifi_multi.run() != WL_CONNECTED) {
      Serial.println("WiFi not connected!");
    } else {
      Serial.println("WiFi connected");
    }

    return;
  }

  request->send(500, "text/plain", "Failed to update Wi-Fi configuration");
}

void ConfigController::handle_delete_wifi(AsyncWebServerRequest* request) {
  if (request->hasParam("ssid", true)) {
    String ssid = request->getParam("ssid", true)->value();

    if (wifi_config.remove_network(ssid.c_str())) {
      request->send(200, "text/plain", "Wi-Fi network removed");

      return;
    }

    request->send(404, "text/plain", "Network not found");

    return;
  }

  request->send(400, "text/plain", "SSID parameter missing");
}
