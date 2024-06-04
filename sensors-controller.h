#ifndef SENSORS_CONTROLLER_H
#define SENSORS_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <unordered_set>

#include "files_model.h"

using pin_t = uint8_t;
using pin_value_t = int;

class SensorsController {
public:
  SensorsController(AsyncWebServer* serverRef);
  ~SensorsController();

  void setup_routes();
  void on_setup();
  void on_loop();

  pin_value_t get_sensor_value(pin_t pin);

private:
  AsyncWebServer* server;
  AsyncWebSocket* ws;

  std::unordered_set<pin_t> pins;

  void notify_clients(pin_t pin, pin_value_t sensor_value);

  void handle_websocket_event(
    AsyncWebSocket* target_ws,
    AsyncWebSocketClient* client,
    AwsEventType type,
    void* arg,
    uint8_t* data,
    size_t len
  );
  void handle_websocket_message(void *arg, uint8_t *data, size_t len);

  void handle_update_pin(AsyncWebServerRequest* request);
};

#endif
