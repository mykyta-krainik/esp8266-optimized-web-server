#include "sensors-controller.h"

SensorsController::SensorsController(AsyncWebServer* serverRef) : server(serverRef) {
  ws = new AsyncWebSocket("/ws");
}

SensorsController::~SensorsController() {
  ws->closeAll();

  delete ws;
  ws = nullptr;
}

void SensorsController::setup_routes() {
  ws->onEvent([this](AsyncWebSocket* target_ws, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    handle_websocket_event(target_ws, client, type, arg, data, len);
  });

  server->addHandler(ws);

  server->on("/update", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_update_pin(request);
  });
}

void SensorsController::on_setup() {
  randomSeed(analogRead(A0));
}

void SensorsController::on_loop() {
  if (ws->count() > 0) {
    ws->cleanupClients();

    for (const pin_t pin : pins) {
      pin_value_t value = get_sensor_value(pin);

      notify_clients(pin, value);
    }

    delay(2000);
  }
}

pin_value_t SensorsController::get_sensor_value(pin_t pin) {
  // return analogRead(pin);

  return random(0, 1024);
}

void SensorsController::notify_clients(pin_t pin, pin_value_t value) {
  JsonDocument doc;

  doc["pin"] = pin;
  doc["value"] = String(value);

  String serialized_doc;

  serializeJson(doc, serialized_doc);

  ws->textAll(serialized_doc);
}

void SensorsController::handle_websocket_message(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if(
    info->final
    && info->index == 0
    && info->len == len
    && info->opcode == WS_TEXT
  ) {
    data[len] = 0;

    Serial.printf("WS received data: %s\n", (char*)data);

    JsonDocument doc;

    deserializeJson(doc, data);

    String action = doc["action"];

    if (action != "start") {
      return;
    }

    pin_t pin = doc["pin"].as<pin_t>();

    if (pins.find(pin) == pins.end()) {
      pins.insert(pin);
    }
  }
}

void SensorsController::handle_websocket_event(
  AsyncWebSocket* target_ws,
  AsyncWebSocketClient* client,
  AwsEventType type,
  void* arg,
  uint8_t* data,
  size_t len
) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());

      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());

      break;

    case WS_EVT_DATA:
      handle_websocket_message(arg, data, len);
      
      break;

    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void SensorsController::handle_update_pin(AsyncWebServerRequest* request) {
  if (!request->hasParam("pin") || !request->hasParam("value")) {
    request->send(400, "text/plain", "There is no pin or value");
  }

  pin_t pin = request->getParam("pin")->value().toInt();
  pin_value_t value = request->getParam("value")->value().toInt();

  pinMode(pin, OUTPUT);
  analogWrite(pin, value);

  request->send(200, "text/plain", "Updated");
}
