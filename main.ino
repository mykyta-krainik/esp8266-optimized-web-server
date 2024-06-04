#include "server.h"

MicroServer server(80);

void setup() {
  server.on_setup();
}

void loop() {
  Serial.println("Before on_loop");

  server.on_loop();

  Serial.println("After on_loop");
}
