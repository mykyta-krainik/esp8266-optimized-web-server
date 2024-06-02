#ifndef FILES_H
#define FILES_H

#include <LittleFS.h>
#include <Arduino.h>

class Files {
public:
  static Files& get_instance();
  bool save_file(const String& path, const uint8_t* data, size_t len);
  bool write_to_file(const String& path, const String data);
  String read_file(const String& path);
  File get_file(const String& path);
  bool delete_file(const String& path);
  bool file_exists(const String& path);
  size_t get_available_space();
  size_t get_total_space();
  bool has_enough_space(const size_t file_size);

private:
  Files();
  ~Files();
  Files(const Files&) = delete;
  Files& operator=(const Files&) = delete;

  void init_littlefs();
};

#endif
