#include "server.h"

MicroServer::MicroServer(uint16_t port) {
  server = new AsyncWebServer(port);

  view_controller = new ViewController(server);
  base_controller = new BaseController(server);
  config_controller = new ConfigController(server);
}

MicroServer::~MicroServer() {
  delete server;
  server = nullptr;

  delete view_controller;
  view_controller = nullptr;

  delete base_controller;
  base_controller = nullptr;

  delete config_controller;
  config_controller = nullptr;
}

void MicroServer::on_setup() {
  ESP8266WiFiMulti wifi_multi;

  Serial.begin(115200);

  if (!init_fs()) {
    Serial.println("Please, restart the SoC!");

    return;
  }

  if (!connect_to_wifi()) {
    Serial.println("Please, restart the SoC!");

    return;
  }

  register_mDNS("esp8266-web-server");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setup_routes();

  server->serveStatic("/static", LittleFS, "/static").setDefaultFile("index.html.gz");
  server->begin();
  Serial.println("HTTP server started.");
}

void MicroServer::on_loop() {
  MDNS.update();
}

void MicroServer::setup_routes() {
  view_controller->setup_routes();
  base_controller->setup_routes();
  config_controller->setup_routes();
}

void MicroServer::register_mDNS(const char* url) {
  if (!MDNS.begin(url)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.printf("mDNS responder started: %s.local\n", url);
  }

  MDNS.addService("http", "tcp", 80);
}

bool MicroServer::connect_to_wifi() {
  ESP8266WiFiMulti wifi_multi;

  File configFile = LittleFS.open("/static/wifi_config.json", "r");

  if (!configFile) {
    Serial.println("Failed to open wifi_config.json");
    return false;
  }

  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  JsonDocument doc;

  deserializeJson(doc, buf.get());

  JsonArray array = doc["networks"];

  for (JsonObject obj : array) {
    const char* ssid = obj["ssid"];
    const char* password = obj["password"];

    wifi_multi.addAP(ssid, password);
  }

  Serial.println("Connecting to Wi-Fi...");

  while (wifi_multi.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to Wi-Fi.");

  return true;
}

bool MicroServer::init_fs() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");

    return false;
  }

  Serial.println("LittleFS mounted successfully");

  return true;
}
