#ifndef FILES_H
#define FILES_H

#include <FS.h>
#include <Arduino.h>

class Files {
public:
    static Files& get_instance();
    bool save_file(const String& path, const uint8_t* data, size_t len);
    String read_file(const String& path);
    bool delete_file(const String& path);
    bool file_exists(const String& path);

private:
    Files();
    ~Files();
    Files(const Files&) = delete;
    Files& operator=(const Files&) = delete;

    void init_spiffs();
};

#endif
