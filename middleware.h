#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <initializer_list>

#include "middleware_context.h"

class Middleware {
public:
  Middleware *next = nullptr;

  virtual ~Middleware() {}

  void setNext(Middleware *n) {
    next = n;
  }

  virtual bool handle(AsyncWebServerRequest* request, MiddlewareContext& context, uint8_t* data) = 0;
};

#endif
