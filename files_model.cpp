#include "files_model.h"

Files& Files::get_instance() {
    static Files instance;
    return instance;
}

Files::Files() {
    init_spiffs();
}

Files::~Files() {
    SPIFFS.end();
}

void Files::init_spiffs() {
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
    } else {
        Serial.println("SPIFFS mounted successfully");
    }
}

bool Files::save_file(const String& path, const uint8_t* data, size_t len) {
  File file = SPIFFS.open(path, "a");
  const String result = (bool(file) ? "true" : "false");

  Serial.println("File: " + result + path);

  if (!file) {
      return false;
  }
  if (data != nullptr && len > 0) {
      size_t written = file.write(data, len);
      file.close();
      return written == len;
  }
  file.close();
  return true;
}

String Files::read_file(const String& path) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading.");
        return "";
    }

    size_t file_size = file.size();
    if (ESP.getFreeHeap() < file_size + 512) {
        Serial.println("Not enough memory to read the file.");
        file.close();
        return "";
    }

    String file_content;
    char buf[512];
    while (file.available()) {
        int bytes_to_read = min(sizeof(buf), (unsigned int)file.available());
        int bytes_read = file.readBytes(buf, bytes_to_read);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
            file_content += String(buf);
        }
    }
    file.close();
    return file_content;
}

bool Files::delete_file(const String& path) {
    return SPIFFS.remove(path);
}

bool Files::file_exists(const String& path) {
    return SPIFFS.exists(path);
}
