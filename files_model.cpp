#include "files_model.h"

Files& Files::get_instance() {
  static Files instance;

  return instance;
}

Files::Files() {
  init_littlefs();
}

Files::~Files() {
  LittleFS.end();
}

void Files::init_littlefs() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

bool Files::write_to_file() {
  return true;
}

bool Files::save_file(const String& path, const uint8_t* data, size_t len) {
  const auto available_space = get_available_space();

  if (available_space < len) {
    LittleFS.remove(path);

    Serial.println("Sorry, there is no available space");

    return false;
  }

  File file = LittleFS.open(path, "a");
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
  File file = LittleFS.open(path, "r");

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

  while (file.available()) {
    char buf[512];
    int bytes_to_read = min(sizeof(buf) - 1, (unsigned int)file.available());
    size_t bytes_read = file.readBytes(buf, bytes_to_read);

    if (bytes_read > 0) {
      buf[bytes_read] = '\0';
      file_content += String(buf);
    }
  }

  file.close();

  return file_content;
}

File Files::get_file(const String &path) {
  File file = LittleFS.open(path, "r");

  if (!file) {
    Serial.println("Failed to open file for reading.");
  }

  return file;
}

bool Files::delete_file(const String& path) {
  if (!file_exists(path)) {
    return false;
  }

  return LittleFS.remove(path);
}

bool Files::file_exists(const String& path) {
  return LittleFS.exists(path);
}

size_t Files::get_available_space() {
  FSInfo fs_info;

  LittleFS.info(fs_info);

  return fs_info.totalBytes - fs_info.usedBytes;
}

size_t Files::get_total_space() {
  FSInfo fs_info;

  LittleFS.info(fs_info);

  return fs_info.totalBytes;
}

bool Files::has_enough_space(const size_t file_size) {
  const size_t available_space = get_available_space();

  return file_size <= available_space;
}
