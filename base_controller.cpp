#include "base_controller.h"

BaseController::BaseController(ESP8266WebServer& serverRef) : server(serverRef) {}

void BaseController::setup_routes() {
  server.on("/", HTTP_GET, std::bind(&BaseController::handleGetUploadPage, this));
  server.on("/list-files", HTTP_GET, std::bind(&BaseController::handleGetFilesList, this));
  server.onNotFound(std::bind(&BaseController::handleNotFound, this));
}

void BaseController::handleGetUploadPage() {
  String path = "/static/base.html.gz";

  Serial.println(Files::get_instance().file_exists(path));

  if (Files::get_instance().file_exists(path)) {
    File html_base = Files::get_instance().get_file(path);

    Serial.println("You are here");

    server.streamFile<File>(html_base, "text/html");

    return;
  }

  server.sendHeader("Location", "/config", true);
  server.send(404, "text/plain", "Oops, there is no base file for working with upload");
}

void BaseController::handleGetFilesList() {
  Dir dir = LittleFS.openDir("/static");
  String fileNames = "<h1>Stored Files</h1><ul>";
  
  while (dir.next()) {
    fileNames += "<li>" + dir.fileName() + "</li>";
  }

  fileNames += "</ul>";

  server.send(200, "text/html", fileNames);
  server.client().stop();
}

void BaseController::handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "No HTML file found.");
}
