#ifndef JSON_MIDDLEWARE_H
#define JSON_MIDDLEWARE_H

#include "middleware.h"
#include "middleware_context.h"

class JsonMiddleware : public Middleware {
public:
  bool handle(ESP8266WebServer& server, MiddlewareContext& context) override {
    if (server.header("Content-Type") != "application/json") {
      if (next) {
        return next->handle(server, context);
      }

      return true;
    }

    DeserializationError error = deserializeJson(context.jsonDoc, server.arg("plain"));

    if (error) {
      Serial.println("Deserialization error: " + String(error.c_str()));

      server.send(400, "text/plain", error.c_str());

      return false;
    }

    if (next) {
      return next->handle(server, context);
    }

    return true;
  }
};

#endif
