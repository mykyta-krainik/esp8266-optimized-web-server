#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include <ESP8266WebServer.h>
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

  virtual bool handle(ESP8266WebServer& server, MiddlewareContext& context) = 0;
};

#endif
