#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>

#include "files_model.h"

class WifiConfig {
public:
  String get_all_networks();
  bool update_network(const char* ssid, const char* password);
  bool remove_network(const char* ssid);
  bool network_exists(const char* ssid);

private:
  const char* file_path = "/static/wifi_config.json";

  void add(const char* ssid, const char* password);
  void update(JsonDocument config);
  void remove(JsonDocument config);

  JsonDocument read();
  void save(JsonDocument doc);
};

#endif
