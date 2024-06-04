#ifndef JSON_MIDDLEWARE_H
#define JSON_MIDDLEWARE_H

#include "middleware.h"
#include "middleware_context.h"

class JsonMiddleware : public Middleware {
public:
  bool handle(AsyncWebServerRequest* request, MiddlewareContext& context, uint8_t* data) override {
    if (!request->contentType().equalsIgnoreCase("application/json")) {
      if (next) {
        return next->handle(request, context, data);
      }

      return true;
    }

    DeserializationError error = deserializeJson(context.jsonDoc, (char*)data);

    if (error) {
      Serial.println("Deserialization error: " + String(error.c_str()));

      request->send(400, "text/plain", error.c_str());

      return false;
    }

    if (next) {
      return next->handle(request, context, data);
    }

    return true;
  }
};

#endif
