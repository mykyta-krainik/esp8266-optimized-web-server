#include "page_controller.h"

PageController::PageController(ESP8266WebServer& serverRef) : server(serverRef) {
}

void PageController::setupRoutes() {
  server.on("/", HTTP_GET, std::bind(&PageController::handleGetRoot, this));
  server.on("/base", HTTP_GET, std::bind(&PageController::handleGetBase, this));
  server.on("/base", HTTP_POST, std::bind(&PageController::handlePostBase, this));
  server.on("/base", HTTP_PATCH, std::bind(&PageController::handlePatchBase, this));
  server.on("/base", HTTP_DELETE, std::bind(&PageController::handleDeleteBase, this));
  server.on("/list-files", HTTP_GET, std::bind(&PageController::handleListFiles, this));
}

bool PageController::isValidFileType(const String& filename) {
    return filename.endsWith(".html") || filename.endsWith(".htm") ||
           filename.endsWith(".css") || filename.endsWith(".js");
}

void PageController::handleGetRoot() {
    String htmlPage = R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Upload HTML Page</title>
        </head>
        <body>
            <h1>Upload HTML Page</h1>
            <form action="/base" method="post" enctype="multipart/form-data">
                <label for="file">Choose HTML file to upload:</label>
                <input type="file" id="file" name="file" accept=".html,.htm" required>
                <br><br>
                <input type="submit" value="Upload">
            </form>
        </body>
        </html>
    )rawliteral";

    server.send(200, "text/html", htmlPage);
}

void PageController::handleGetBase() {
    String path = "/index.html";
    if (Files::get_instance().file_exists(path)) {
        String htmlContent = Files::get_instance().read_file(path);
        server.send(200, "text/html", htmlContent);
    } else {
        server.send(404, "text/plain", "No HTML file found.");
    }
}

void PageController::handlePostBase() {
    HTTPUpload& upload = server.upload();
    String filename = "/" + upload.filename;

    if (upload.status == UPLOAD_FILE_START) {
        if (!isValidFileType(filename)) {
            server.send(415, "text/plain", "Unsupported file type.");
            return;
        }

        if (!Files::get_instance().file_exists(filename)) {
            Files::get_instance().save_file(filename, nullptr, 0);
        }
    }

    if (upload.status == UPLOAD_FILE_WRITE) {
        Files::get_instance().save_file(filename, upload.buf, upload.currentSize);
    }

    if (upload.status == UPLOAD_FILE_END) {
        if (upload.currentSize > 0) {
          Files::get_instance().save_file(filename, upload.buf, upload.currentSize);
        }
        server.send(200, "text/plain", "File successfully uploaded.");
    }
}

void PageController::handleListFiles() {
    Dir dir = SPIFFS.openDir("/");
    String fileNames = "<h1>Stored Files</h1><ul>";
    
    while (dir.next()) {
        fileNames += "<li>" + dir.fileName() + "</li>";
    }
    fileNames += "</ul>";

    server.send(200, "text/html", fileNames);
}

void PageController::handlePatchBase() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        Serial.print("Patch File Upload Name: "); Serial.println(filename);

        if (!isValidFileType(filename)) {
            Serial.println("Invalid file type for patching.");
            server.send(415, "text/plain", "Unsupported file type for patching.");
            return;
        }

        if (!Files::get_instance().file_exists(filename)) {
            Serial.println("File does not exist, cannot patch.");
            server.send(404, "text/plain", "File not found for patching.");
            return;
        }

        fsUploadFile = SPIFFS.open(filename, "w");
        if (!fsUploadFile) {
            Serial.println("Failed to open file for patching.");
            server.send(500, "text/plain", "Failed to open file for patching.");
            return;
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile) {
            fsUploadFile.write(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile) {
            fsUploadFile.close();
            Serial.print("File Patched: "); Serial.println(upload.filename);
            server.send(200, "text/plain", "File successfully patched.");
        } else {
            server.send(500, "text/plain", "Error during file patching.");
        }
    }
}

void PageController::handleDeleteBase() {
    if (server.hasArg("type")) {
        String fileType = server.arg("type");
        String filePath = "/" + fileType + ".css";
        if (Files::get_instance().file_exists(filePath)) {
            Files::get_instance().delete_file(filePath);
        }
    } else {
        Files::get_instance().delete_file("/index.html");
        Files::get_instance().delete_file("/style.css");
        Files::get_instance().delete_file("/script.js");
    }
    server.send(200, "text/plain", "Files deleted.");
}

