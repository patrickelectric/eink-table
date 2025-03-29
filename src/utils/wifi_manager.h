#pragma once

#include <WiFi.h>
#include "debug.h"
#include "network.h"

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
                    debug("Connecting to %s: %s", wifi->ssid,
                        Network::wifiStatusToString(WiFi.status()).c_str());
                }

                if (WiFi.status() == WL_CONNECTED) {
                    debug("WiFi connected! IP: %s", WiFi.localIP().toString().c_str());
                }
            }
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    WiFiManager() : initialized(false), wifiTaskHandle(nullptr), ssid(nullptr), password(nullptr) {}

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
                String rssiStr = String(WiFi.RSSI(i)) + " dBm";
                debug("%-32s | %-12s | %-8s",
                    WiFi.SSID(i).c_str(),
                    Network::encryptionTypeToString(WiFi.encryptionType(i)).c_str(),
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
