#include "base_controller.h"

BaseController::BaseController(ESP8266WebServer& serverRef) : server(serverRef) {}

void BaseController::setup_routes() {
  server.on("/", HTTP_GET, std::bind(&BaseController::handle_get_upload_page, this));
  server.on("/", HTTP_POST, std::bind(&BaseController::handle_post_upload_page, this));
  server.on("/list-files", HTTP_GET, std::bind(&BaseController::handle_get_files_list, this));
  server.onNotFound(std::bind(&BaseController::handle_not_found, this));
}

bool BaseController::is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types) {
  for (size_t i = 0; i < acceptable_file_types.size(); i++) {
    if (filename.endsWith(acceptable_file_types[i])) {
      return true;
    }
  }

  return false;
}

void BaseController::handle_upload_file(const String& path_to_save, std::vector<const char*> valid_types) {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    if (!is_file_type_valid(upload.filename, valid_types)) {
      server.send(415, "text/plain", "Unsupported file type");
      server.client().stop();

      return;
    }

    if (!Files::get_instance().has_enough_space(upload.contentLength)) {
      server.send(413, "text/plain", "File is too large");
      server.client().stop();

      return;
    }

    if (Files::get_instance().file_exists(path_to_save)) {
      Files::get_instance().delete_file(path_to_save);
    }

    if (!Files::get_instance().file_exists(path_to_save)) {
      Files::get_instance().save_file(path_to_save, nullptr, 0);
    }
  }

  if (upload.status == UPLOAD_FILE_WRITE) {
    Files::get_instance().save_file(path_to_save, upload.buf, upload.currentSize);
  }

  if (upload.status == UPLOAD_FILE_END) {
    File page = Files::get_instance().get_file(path_to_save);

    if (page && page.size() != upload.totalSize) {
      Files::get_instance().delete_file(path_to_save);

      server.send(500, "text/plain", "Failed saving: Uploaded file has been corrupted");
      server.client().stop();
    }
  }
}

void BaseController::handle_get_upload_page() {
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

void BaseController::handle_post_upload_page() {
  handle_upload_file("/static/base.html.gz", { ".html.gz" });
}

void BaseController::handle_get_files_list() {
  Dir dir = LittleFS.openDir("/static");
  String fileNames = "<h1>Stored Files</h1><ul>";
  
  while (dir.next()) {
    fileNames += "<li>" + dir.fileName() + "</li>";
  }

  fileNames += "</ul>";

  server.send(200, "text/html", fileNames);
  server.client().stop();
}

void BaseController::handle_not_found() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "No HTML file found.");
}
