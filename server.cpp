#include "server.h"
// TODO: #include "config_controller.h"

MicroServer::MicroServer(unsigned int port) {
  server = new ESP8266WebServer(port);
  view_controller = new ViewController(*server);
  base_controller = new BaseController(*server);
  // TODO: config_controller = new ConfigController(*server);
}

MicroServer::~MicroServer() {
  delete server;
  server = nullptr;

  delete view_controller;
  view_controller = nullptr;

  delete base_controller;
  base_controller = nullptr;

  // delete config_controller;
  // config_controller = nullptr;
}

void MicroServer::on_setup() {
  Serial.begin(115200);

  if (!connect_to_wifi()) {
    Serial.println("Please, restart the SoC!");

    return;
  }

  register_mDNS("esp8266-web-server");

  Serial.printf("IP address: %s\n", WiFi.localIP());

  if (!init_fs()) {
    Serial.println("Please, restart the SoC!");

    return;
  }

  setup_routes();

  server->serveStatic("/static", LittleFS, "/static");
  server->begin();
  Serial.println("HTTP server started.");
}

void MicroServer::on_loop() {
  server->handleClient();
  MDNS.update();
}

void MicroServer::setup_routes() {
  view_controller->setup_routes();
  base_controller->setup_routes();
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
  const char* ssid = "Kuriyama";
  const char* password = "24051981krlit";

  WiFi.begin(ssid, password);

  Serial.println("Connecting to Wi-Fi.");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");

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
