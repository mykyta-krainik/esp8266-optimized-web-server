#ifndef MIDDLEWARE_CHAIN_H
#define MIDDLEWARE_CHAIN_H

#include <ESPAsyncWebServer.h>
#include <initializer_list>

#include "middleware.h"
#include "middleware_context.h"

class MiddlewareChain {
public:
  MiddlewareChain(std::initializer_list<Middleware*> middlewares) {
    for (Middleware* mw : middlewares) {
      add(mw);
    }
  }

  void add(Middleware* mw) {
    if (!head) {
      head = mw;
      tail = mw;

      return;
    }

    tail->setNext(mw);
    tail = mw;
  }

  bool execute(AsyncWebServerRequest* request, MiddlewareContext& context, uint8_t* data) {
    if (head) {
      return head->handle(request, context, data);
    }

    return false;
  }

private:
  Middleware* head = nullptr;
  Middleware* tail = nullptr;
};

#endif
