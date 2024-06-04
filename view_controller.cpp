#include "view_controller.h"

ViewController::ViewController(AsyncWebServer* serverRef) : server(serverRef) {}

void ViewController::setup_routes() {
  server->on("/view", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_view(request);
  });

  server->on("/view", HTTP_POST, 
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Uploaded");
    },
    [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      handle_upload_file("/static/index.html.gz", {".html.gz"}, request, filename, index, data, len, final);
    }
  );

  server->on("/view", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    handle_delete_view(request);
  });

  server->on("/css", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_css(request);
  });

  server->on("/css", HTTP_POST, 
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Uploaded");
    },
    [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      handle_upload_file("/static/styles.css.gz", {".css.gz"}, request, filename, index, data, len, final);
    }
  );

  server->on("/css", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    handle_delete_css(request);
  });

  server->on("/js", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_js(request);
  });

  server->on("/js", HTTP_POST, 
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Uploaded");
    },
    [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      handle_upload_file("/static/index.js.gz", {".js.gz"}, request, filename, index, data, len, final);
    }
  );

  server->on("/js", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    handle_delete_js(request);
  });
}

bool ViewController::is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types) {
  for (size_t i = 0; i < acceptable_file_types.size(); i++) {
    if (filename.endsWith(acceptable_file_types[i])) {
      return true;
    }
  }

  return false;
}

void ViewController::handle_upload_file(
  const String& path_to_save,
  std::vector<const char*> valid_types,
  AsyncWebServerRequest *request,
  const String& filename,
  size_t index,
  uint8_t *data,
  size_t len,
  bool final
) {
  if (!is_file_type_valid(filename, valid_types)) {
    request->send(415, "text/plain", "Unsupported file type");
    return;
  }

  if (index == 0) {
    if (!Files::get_instance().has_enough_space(request->contentLength())) {
      request->send(413, "text/plain", "File is too large");

      return;
    }

    if (Files::get_instance().file_exists(path_to_save)) {
      Files::get_instance().delete_file(path_to_save);
    }
  }

  Files::get_instance().save_file(path_to_save, data, len);
}

void ViewController::handle_get_view(AsyncWebServerRequest* request) {
  String path = "/static/index.html.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    AsyncWebServerResponse* response = request->beginResponse(page, path, "text/html");

    response->addHeader("Content-Encoding", "gzip");

    request->send(response);

    return;
  }

  request->redirect("/");
}

void ViewController::handle_delete_view(AsyncWebServerRequest* request) {
  const bool result = Files::get_instance().delete_file("/static/index.html.gz");

  if (!result) {
    request->send(500, "text/plain", "File doesn't exist or smth went wrong");

    return;
  }

  request->send(200, "text/plain", "View has been deleted");
}

void ViewController::handle_get_css(AsyncWebServerRequest* request) {
  String path = "/static/styles.css.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    AsyncWebServerResponse* response = request->beginResponse(page, path, "text/css");

    response->addHeader("Content-Encoding", "gzip");

    request->send(response);

    return;
  }

  request->redirect("/");
}

void ViewController::handle_delete_css(AsyncWebServerRequest* request) {
  const bool result = Files::get_instance().delete_file("/static/styles.css.gz");

  if (!result) {
    request->send(500, "text/plain", "File doesn't exist or smth went wrong");

    return;
  }

  request->send(200, "text/plain", "View has been deleted");
}

void ViewController::handle_get_js(AsyncWebServerRequest* request) {
  String path = "/static/index.js.gz";

  if (Files::get_instance().file_exists(path)) {
    File page = LittleFS.open(path, "r");

    AsyncWebServerResponse* response = request->beginResponse(page, path, "text/js");

    response->addHeader("Content-Encoding", "gzip");

    request->send(response);

    return;
  }

  request->redirect("/");
}

void ViewController::handle_delete_js(AsyncWebServerRequest* request) {
  const bool result = Files::get_instance().delete_file("/static/index.js.gz");

  if (!result) {
    request->send(500, "text/plain", "File doesn't exist or smth went wrong");

    return;
  }

  request->send(200, "text/plain", "View has been deleted");
}
