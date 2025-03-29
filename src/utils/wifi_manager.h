#pragma once

#include <WiFi.h>
#include "debug.h"

class WiFiManager {
private:
    static WiFiManager* instance;
    const char* ssid;
    const char* password;
    bool initialized;
    TaskHandle_t wifiTaskHandle;

    static void wifiTask(void* parameter) {
        WiFiManager* wifi = (WiFiManager*)parameter;
        while(true) {
            if (WiFi.status() != WL_CONNECTED) {
                debug("WiFi disconnected. Attempting to reconnect...");
                wifi->scan();
                WiFi.begin(wifi->ssid, wifi->password);

                int attempts = 0;
                while (WiFi.status() != WL_CONNECTED && attempts < 10) {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    attempts++;
                    debug("Connecting to %s: %s", wifi->ssid, wifi->getStatusString().c_str());
                }

                if (WiFi.status() == WL_CONNECTED) {
                    debug("WiFi connected! IP: %s", WiFi.localIP().toString().c_str());
                }
            }
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    WiFiManager() : initialized(false), wifiTaskHandle(nullptr), ssid(nullptr), password(nullptr) {}

    String getStatusString() const {
        switch (WiFi.status()) {
            case WL_CONNECTED:
                return "Connected";
            case WL_NO_SHIELD:
                return "No WiFi shield";
            case WL_IDLE_STATUS:
                return "Idle";
            case WL_NO_SSID_AVAIL:
                return "No SSID available";
            case WL_SCAN_COMPLETED:
                return "Scan completed";
            case WL_CONNECT_FAILED:
                return "Connection failed";
            case WL_CONNECTION_LOST:
                return "Connection lost";
            case WL_DISCONNECTED:
                return "Disconnected";
            default:
                return "Unknown";
        }
    }

public:
    static WiFiManager* self() {
        if (instance == nullptr) {
            instance = new WiFiManager();
        }
        return instance;
    }

    void begin(const char* wifi_ssid, const char* wifi_password) {
        if (!initialized) {
            ssid = wifi_ssid;
            password = wifi_password;

            WiFi.mode(WIFI_STA);
            debug("Starting WiFi connection to %s", ssid);

            xTaskCreate(
                wifiTask,
                "WiFiManager",
                4096,
                this,
                1,
                &wifiTaskHandle
            );

            initialized = true;
            debug("WiFi manager task created");
        }
    }

    bool isConnected() const {
        return WiFi.status() == WL_CONNECTED;
    }

    String getIP() const {
        return WiFi.localIP().toString();
    }

    void scan() {
          debug("Scanning for networks...");
        int n = WiFi.scanNetworks();

        if (n == 0) {
            debug("No networks found");
        } else {
            debug("\nNetworks found:");
            debug("%-32s | %-12s | %-8s", "SSID", "Security", "RSSI");
            debug("----------------------------------------------------------------");

            for (int i = 0; i < n; ++i) {
                String security;
                switch(WiFi.encryptionType(i)) {
                    case WIFI_AUTH_OPEN:
                        security = "Open";
                        break;
                    case WIFI_AUTH_WEP:
                        security = "WEP";
                        break;
                    case WIFI_AUTH_WPA_PSK:
                        security = "WPA-PSK";
                        break;
                    case WIFI_AUTH_WPA2_PSK:
                        security = "WPA2-PSK";
                        break;
                    case WIFI_AUTH_WPA_WPA2_PSK:
                        security = "WPA/WPA2";
                        break;
                    case WIFI_AUTH_WPA2_ENTERPRISE:
                        security = "WPA2-ENT";
                        break;
                    default:
                        security = "Unknown";
                }

                String rssiStr = String(WiFi.RSSI(i)) + " dBm";
                debug("%-32s | %-12s | %-8s",
                    WiFi.SSID(i).c_str(),
                    security.c_str(),
                    rssiStr.c_str()
                );
            }
            debug("----------------------------------------------------------------");
        }
        WiFi.scanDelete();
    }

    ~WiFiManager() {
        if (wifiTaskHandle != nullptr) {
            vTaskDelete(wifiTaskHandle);
        }
        if (initialized) {
            WiFi.disconnect(true);
        }
    }

    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;
};

WiFiManager* WiFiManager::instance = nullptr;
