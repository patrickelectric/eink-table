#pragma once

#include <WebServer.h>
#include "debug.h"

class WebServerManager {
private:
    static WebServerManager* instance;
    WebServer server;
    bool initialized;
    TaskHandle_t serverTaskHandle;

    static void serverTask(void* parameter) {
        WebServerManager* server = (WebServerManager*)parameter;
        while(true) {
            server->handle();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    WebServerManager() : server(80), initialized(false), serverTaskHandle(nullptr) {
        setupRoutes();
    }

    void setupRoutes() {
        server.on("/", HTTP_GET, [this]() { handleRoot(); });
        server.on("/logs", HTTP_GET, [this]() { handleLogRequest(); });
        
        // Handle 404
        server.onNotFound([this]() {
            server.send(404, "text/plain", "File Not Found");
        });
    }

    void handleRoot() {
        String html = "<!DOCTYPE html><html><head>";
        html += "<title>ESP32 Log Viewer</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 20px; }";
        html += ".log-link { display: block; margin: 10px 0; padding: 10px; background-color: #f0f0f0; text-decoration: none; color: #333; border-radius: 5px; }";
        html += ".log-link:hover { background-color: #e0e0e0; }";
        html += "</style>";
        html += "</head><body>";
        html += "<h1>ESP32 Log Files</h1>";

        std::vector<String> logFiles = Logger::getInstance()->listLogFiles();
        
        if (logFiles.empty()) {
            html += "<p>No log files found</p>";
        } else {
            for (const auto& file : logFiles) {
                html += "<a class='log-link' href='/logs?file=" + file + "'>" + file + "</a>";
            }
        }

        html += "</body></html>";
        server.send(200, "text/html", html);
    }

    void handleLogRequest() {
        String filename = server.arg("file");
        if (filename.length() == 0) {
            server.send(400, "text/plain", "File parameter is required");
            return;
        }

        String content = Logger::getInstance()->readLogFile(filename);

        String html = "<!DOCTYPE html><html><head>";
        html += "<title>Log File: " + filename + "</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 20px; }";
        html += "pre { background-color: #f0f0f0; padding: 15px; border-radius: 5px; white-space: pre-wrap; }";
        html += ".back-link { display: inline-block; margin-bottom: 20px; color: #0066cc; text-decoration: none; }";
        html += "</style>";
        html += "</head><body>";
        html += "<a class='back-link' href='/'>&larr; Back to file list</a>";
        html += "<h1>Log File: " + filename + "</h1>";
        html += "<pre>" + content + "</pre>";
        html += "</body></html>";

        server.send(200, "text/html", html);
    }

public:
    static WebServerManager* self() {
        if (instance == nullptr) {
            instance = new WebServerManager();
        }
        return instance;
    }

    void begin() {
        if (!initialized) {
            server.begin();
            initialized = true;
            debug("Web server started on port 80");

            xTaskCreate(
                serverTask,
                "WebServer",
                8192,
                this,
                1,
                &serverTaskHandle
            );
            debug("Web server task created");
        }
    }

    void handle() {
        server.handleClient();
    }

    ~WebServerManager() {
        if (serverTaskHandle != nullptr) {
            vTaskDelete(serverTaskHandle);
        }
        if (initialized) {
            server.stop();
        }
    }

    WebServerManager(const WebServerManager&) = delete;
    WebServerManager& operator=(const WebServerManager&) = delete;
};

WebServerManager* WebServerManager::instance = nullptr;
