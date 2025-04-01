#pragma once

#include <LittleFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "debug.h"

class WebServerManager {
private:
    static WebServerManager* instance;
    WebServer server;
    WebSocketsServer webSocket;
    bool initialized;
    TaskHandle_t serverTaskHandle;

    static void serverTask(void* parameter) {
        WebServerManager* server = (WebServerManager*)parameter;
        while(true) {
            server->handle();
            server->webSocket.loop();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    WebServerManager()
        : server(80)
        , webSocket(81)
        , initialized(false)
        , serverTaskHandle(nullptr)
    {
        setupRoutes();
        setupWebSocket();
    }

    void setupWebSocket() {
        webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
            switch(type) {
                case WStype_DISCONNECTED:
                    debug("WebSocket client #%u disconnected", num);
                    break;
                case WStype_CONNECTED:
                    {
                        webSocket.sendTXT(num, "Connected to ESP32 WebSocket Server\n");
                        debug("WebSocket client #%u connected", num);
                    }
                    break;
                default:
                    break;
            }
        });
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
        html += "#logArea { height: 400px; overflow-y: auto; background-color: #f0f0f0; padding: 15px; border-radius: 5px; font-family: monospace; white-space: pre-wrap; }";
        html += "</style>";
        html += "<script>";
        html += "let ws = new WebSocket('ws://' + window.location.hostname + ':81/');";
        html += "ws.onmessage = function(event) {";
        html += "    const logArea = document.getElementById('logArea');";
        html += "    logArea.innerHTML += event.data;";
        html += "    logArea.scrollTop = logArea.scrollHeight;";
        html += "};";
        html += "</script>";
        html += "</head><body>";
        html += "<h1>ESP32 Log Viewer</h1>";
        html += "<div id='logArea'></div>";
        html += "<h2>Log Files</h2>";

        std::vector<String> logFiles = listFiles();
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

        String content = readFile(filename);

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

    std::vector<String> listFiles() {
        std::vector<String> files;

        File root = LittleFS.open("/");
        File file = root.openNextFile();

        while(file) {
            String fname = file.name();
            if (fname.startsWith("log_") && fname.endsWith(".txt")) {
                files.push_back("/" + fname);
            }
            file = root.openNextFile();
        }

        return files;
    }

    String readFile(String filename) {
        File file = LittleFS.open(filename, "r");
        String content = file.readString();
        file.close();
        return content;
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
            webSocket.begin();
            initialized = true;
            debug("Web server started on port 80");
            debug("WebSocket server started on port 81");

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

    void log(const String& message) {
        if (initialized) {
            String formattedMsg = "[" + String(millis()) + "] " + message;
            webSocket.broadcastTXT(formattedMsg);
        }
    }

    ~WebServerManager() {
        if (serverTaskHandle != nullptr) {
            vTaskDelete(serverTaskHandle);
        }
        if (initialized) {
            webSocket.close();
            server.stop();
        }
    }

    WebServerManager(const WebServerManager&) = delete;
    WebServerManager& operator=(const WebServerManager&) = delete;
};

WebServerManager* WebServerManager::instance = nullptr;
