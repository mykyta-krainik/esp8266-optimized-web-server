#include "base_controller.h"

BaseController::BaseController(AsyncWebServer* serverRef) : server(serverRef) {}

void BaseController::setup_routes() {
  server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_upload_page(request);
  });

  server->on(
    "/",
    HTTP_POST,
    [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Uploaded");
    },
    [this](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
      handle_upload_file("/static/base.html.gz", {".html.gz"}, request, filename, index, data, len, final);
    }
  );

  server->on("/list-files", HTTP_GET, [this](AsyncWebServerRequest* request) {
    handle_get_files_list(request);
  });

  server->onNotFound([this](AsyncWebServerRequest* request) {
    handle_not_found(request);
  });
}

bool BaseController::is_file_type_valid(const String& filename, std::vector<const char*> acceptable_file_types) {
  for (const auto& file_type : acceptable_file_types) {
    if (filename.endsWith(file_type)) {
        return true;
    }
  }

  return false;
}

void BaseController::handle_upload_file(
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

  if (final) {
    File page = Files::get_instance().get_file(path_to_save);

    if (!page || page.size() != request->contentLength()) {
      Files::get_instance().delete_file(path_to_save);

      request->send(500, "text/plain", "Failed saving: Uploaded file has been corrupted");
    }
  }
}

void BaseController::handle_get_upload_page(AsyncWebServerRequest* request) {
  String path = "/static/base.html.gz";

  if (Files::get_instance().file_exists(path)) {
    File html_base = Files::get_instance().get_file(path);

    AsyncWebServerResponse* response = request->beginResponse(html_base, path, "text/html");

    response->addHeader("Content-Encoding", "gzip");

    request->send(response);

    return;
  }

  request->redirect("/config");
}

void BaseController::handle_get_files_list(AsyncWebServerRequest* request) {
  Dir dir = LittleFS.openDir("/static");
  String fileNames = "<h1>Stored Files</h1><ul>";
  
  while (dir.next()) {
    fileNames += "<li>" + dir.fileName() + "</li>";
  }

  fileNames += "</ul>";

  request->send(200, "text/html", fileNames);
}

void BaseController::handle_not_found(AsyncWebServerRequest* request) {
  request->redirect("/");
}
