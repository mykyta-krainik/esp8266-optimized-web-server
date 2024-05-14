#ifndef PAGE_CONTROLLER_H
#define PAGE_CONTROLLER_H

#include <ESP8266WebServer.h>
#include <FS.h>
#include "files_model.h"

class PageController {
public:
    PageController(ESP8266WebServer& server);
    void setupRoutes();

private:
    ESP8266WebServer& server;
    File fsUploadFile;

    void handleGetRoot();
    void handleGetBase();
    void handlePostBase();
    void handlePatchBase();
    void handleDeleteBase();
    void handleListFiles();
    bool isValidFileType(const String& filename);
};

#endif
