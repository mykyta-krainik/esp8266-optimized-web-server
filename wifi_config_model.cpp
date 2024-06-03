#include "wifi_config_model.h"

void WifiConfig::add(const char* ssid, const char* password) {
  ESP8266WiFiMulti wifi_multi;

  wifi_multi.addAP(ssid, password);
}

void WifiConfig::update(JsonDocument config) {
  ESP8266WiFiMulti wifi_multi;

  wifi_multi.cleanAPlist();

  JsonArray array = config["networks"];

  for (JsonObject obj : array) {
    const char* ssid = obj["ssid"];
    const char* password = obj["password"];

    wifi_multi.addAP(ssid, password);
  }
}

void WifiConfig::remove(JsonDocument config) {
  ESP8266WiFiMulti wifi_multi;

  wifi_multi.cleanAPlist();

  JsonArray array = config["networks"];

  for (JsonObject obj : array) {
    const char* ssid = obj["ssid"];
    const char* password = obj["password"];

    wifi_multi.addAP(ssid, password);
  }
}

JsonDocument WifiConfig::read() {
  String result = Files::get_instance().read_file(file_path);
  JsonDocument doc;

  deserializeJson(doc, result);

  return doc;
}

void WifiConfig::save(JsonDocument doc) {
  Files::get_instance().delete_file(file_path);

  String data;

  serializeJson(doc, data);

  Files::get_instance().write_to_file(file_path, data);
}

String WifiConfig::get_all_networks() {
  File configFile = LittleFS.open("/static/wifi_config.json", "r");

  if (!configFile) {
    Serial.println("Failed to open wifi_config.json");
    return "";
  }

  String json = configFile.readString();

  return json.substring(0, json.length() - 1) + "}";
}

bool WifiConfig::update_network(const char* ssid, const char* password) {
  JsonDocument doc = read();
  JsonArray array = doc["networks"];

  for (JsonObject obj : array) {
    if (strcmp(obj["ssid"], ssid) == 0) {
      obj["password"] = password;
      update(doc);
      save(doc);
    
      return true;
    }
  }

  JsonDocument new_config;

  new_config["ssid"] = ssid;
  new_config["password"] = password;

  add(ssid, password);

  array.add(new_config);

  save(doc);

  return true;
}

bool WifiConfig::remove_network(const char* ssid) {
  JsonDocument doc = read();
  JsonArray array = doc["networks"];

  for (size_t i = 0; i < array.size(); i++) {
    if (strcmp(array[i]["ssid"], ssid) == 0) {
      array.remove(i);
      save(doc);

      return true;
    }
  }

  return false;
}

bool WifiConfig::network_exists(const char* ssid) {
  JsonDocument doc = read();
  JsonArray array = doc["networks"];

  for (JsonObject obj : array) {
    if (strcmp(obj["ssid"], ssid) == 0) {
      return true;
    }
  }

  return false;
}
