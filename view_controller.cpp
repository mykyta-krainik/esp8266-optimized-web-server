#include "view_controller.h"

ViewController::ViewController(ESP8266WebServer& serverRef) : server(serverRef) {}

void ViewController::setup_routes() {
  server.on("/view", HTTP_GET, std::bind(&ViewController::handle_get_view, this));
  server.on("/view", HTTP_POST, [this]() {
    server.send(200, "text/plain", "Uploaded");
    server.client().stop();
  }, std::bind(&ViewController::handle_post_view, this));
  server.on("/view", HTTP_DELETE, std::bind(&ViewController::handle_delete_view, this));

  server.on("/css", HTTP_GET, std::bind(&ViewController::handle_get_css, this));
  server.on("/css", HTTP_POST, [this]() {
    server.send(200, "text/plain", "Uploaded");
    server.client().stop();
  }, std::bind(&ViewController::handle_post_css, this));
  server.on("/css", HTTP_DELETE, std::bind(&ViewController::handle_delete_css, this));

  server.on("/js", HTTP_GET, std::bind(&ViewController::handle_get_js, this));
  server.on("/js", HTTP_POST, [this]() {
    server.send(200, "text/plain", "Uploaded");
    server.client().stop();
  }, std::bind(&ViewController::handle_post_js, this));
  server.on("/js", HTTP_DELETE, std::bind(&ViewController::handle_delete_js, this));
}

bool ViewController::is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types) {
  for (size_t i = 0; i < acceptable_file_types.size(); i++) {
    if (filename.endsWith(acceptable_file_types[i])) {
      return true;
    }
  }

  return false;
}

void ViewController::handle_upload_file(const String& path_to_save, std::vector<const char*> valid_types) {
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

void ViewController::handle_get_view() {
  String path = "/static/index.html.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    server.streamFile<File>(page, "text/html");
    server.client().stop();

    return;
  }

  server.sendHeader("Location", "/", true);
  server.send(307, "text/plain", "No HTML file found.");
  server.client().stop();
}

void ViewController::handle_post_view() {
  handle_upload_file("/static/index.html.gz", { ".html.gz" });
}

void ViewController::handle_delete_view() {
  const bool result = Files::get_instance().delete_file("/static/index.html.gz");

  if (!result) {
    server.send(500, "text/plain", "File doesn't exist or smth went wrong");
    server.client().stop();

    return;
  }

  server.send(200, "text/plain", "View has been deleted");
  server.client().stop();
}

void ViewController::handle_get_css() {
  String path = "/static/styles.css.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    server.streamFile<File>(page, "text/css");
    server.client().stop();

    return;
  }

  server.sendHeader("Location", "/", true);
  server.send(307, "text/plain", "No CSS file found.");
  server.client().stop();
}

void ViewController::handle_post_css() {
  handle_upload_file("/static/styles.css.gz", { ".css.gz" });
}

void ViewController::handle_delete_css() {
  const bool result = Files::get_instance().delete_file("/static/styles.css.gz");

  if (!result) {
    server.send(500, "text/plain", "File doesn't exist or smth went wrong");
    server.client().stop();

    return;
  }

  server.send(200, "text/plain", "View has been deleted");
  server.client().stop();
}

void ViewController::handle_get_js() {
  String path = "/static/index.js.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    server.streamFile<File>(page, "text/javascript");
    server.client().stop();

    return;
  }

  server.sendHeader("Location", "/", true);
  server.send(307, "text/plain", "No JS file found.");
  server.client().stop();
}

void ViewController::handle_post_js() {
  handle_upload_file("/static/index.js.gz", { ".js.gz" });
}

void ViewController::handle_delete_js() {
  const bool result = Files::get_instance().delete_file("/static/index.js.gz");

  if (!result) {
    server.send(500, "text/plain", "File doesn't exist or smth went wrong");
    server.client().stop();

    return;
  }

  server.send(200, "text/plain", "View has been deleted");
  server.client().stop();
}
