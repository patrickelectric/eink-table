#pragma once

#include <WiFi.h>
#include <vector>
#include <optional>
#include "debug.h"
#include "network.h"

struct WiFiCredentials {
    String ssid;
    String password;

    WiFiCredentials(const String& s, const String& p) : ssid(s), password(p) {}
};

class WiFiManager {
private:
    static WiFiManager* instance;
    std::vector<WiFiCredentials> credentials;
    std::optional<WiFiCredentials> current_credential;
    bool initialized;
    TaskHandle_t wifiTaskHandle;

    static void wifiTask(void* parameter) {
        WiFiManager* wifi = (WiFiManager*)parameter;
        while(true) {
            if (WiFi.status() != WL_CONNECTED) {
                debug("WiFi disconnected. Attempting to reconnect...");
                wifi->scan();

                bool connected = false;

                // Try current network first if we have one
                if (wifi->current_credential) {
                    if (tryConnect(*wifi->current_credential)) {
                        connected = true;
                    }
                }

                // Try other networks if current failed
                if (!connected) {
                    for (const auto& cred : wifi->credentials) {
                        if (!wifi->current_credential || cred.ssid != wifi->current_credential->ssid) {
                            if (tryConnect(cred)) {
                                wifi->current_credential = cred;
                                connected = true;
                                break;
                            }
                        }
                    }
                }

                if (!connected) {
                    debug("Failed to connect to any network");
                }
            }
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    static bool tryConnect(const WiFiCredentials& cred) {
        debug("Attempting connection to %s", cred.ssid.c_str());
        WiFi.begin(cred.ssid.c_str(), cred.password.c_str());

        for (int attempts = 0; attempts < 10; ++attempts) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            debug("Connecting to %s: %s", cred.ssid.c_str(),
                Network::wifiStatusToString(WiFi.status()).c_str());

            if (WiFi.status() == WL_NO_SSID_AVAIL) {
                debug("No SSID available for %s", cred.ssid.c_str());
                return false;
            }

            if (WiFi.status() == WL_CONNECTED) {
                debug("WiFi connected to %s! IP: %s",
                    cred.ssid.c_str(), WiFi.localIP().toString().c_str());
                return true;
            }
        }

        debug("Failed to connect to %s, trying next network", cred.ssid.c_str());
        return false;
    }

    WiFiManager() : initialized(false), wifiTaskHandle(nullptr) {}

public:
    static WiFiManager* self() {
        if (instance == nullptr) {
            instance = new WiFiManager();
        }
        return instance;
    }

    void begin(const std::vector<WiFiCredentials>& wifi_credentials) {
        if (!initialized) {
            credentials = wifi_credentials;
            if (credentials.empty()) {
                debug("No WiFi credentials provided!");
                return;
            }

            WiFi.mode(WIFI_STA);
            debug("Starting WiFi manager with %d networks", credentials.size());

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

    void begin(std::initializer_list<WiFiCredentials> wifi_credentials) {
        begin(std::vector<WiFiCredentials>(wifi_credentials));
    }

    void begin(const String& wifi_ssid, const String& wifi_password) {
        begin(std::vector<WiFiCredentials>{WiFiCredentials(wifi_ssid, wifi_password)});
    }

    bool isConnected() const {
        return WiFi.status() == WL_CONNECTED;
    }

    String getIP() const {
        return WiFi.localIP().toString();
    }

    String getCurrentSSID() const {
        return current_credential ? current_credential->ssid : String();
    }

    void scan() {
        debug("Scanning for networks...");
        const int network_count = WiFi.scanNetworks();

        if (network_count == 0) {
            debug("No networks found");
            return;
        }

        debug("\nNetworks found:");
        debug("%-32s | %-12s | %-8s | %s", "SSID", "Security", "RSSI", "Known");
        debug("----------------------------------------------------------------------------");

        for (int i = 0; i < network_count; ++i) {
            const String ssid = WiFi.SSID(i);
            const String rssiStr = String(WiFi.RSSI(i)) + " dBm";

            // Check if this is one of our configured networks
            const bool isKnown = std::any_of(credentials.begin(), credentials.end(),
                [&ssid](const auto& cred) { return ssid == cred.ssid; });

            debug("%-32s | %-12s | %-8s | %s",
                ssid.c_str(),
                Network::encryptionTypeToString(WiFi.encryptionType(i)).c_str(),
                rssiStr.c_str(),
                isKnown ? "Yes" : "No"
            );
        }
        debug("----------------------------------------------------------------------------");
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
