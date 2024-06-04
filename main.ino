#include "server.h"

MicroServer server(80);

void setup() {
  server.on_setup();
}

void loop() {
  server.on_loop();
}
