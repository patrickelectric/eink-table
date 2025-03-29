#pragma once

#include <WiFi.h>

namespace Network {
    inline String wifiStatusToString(wl_status_t status) {
        switch (status) {
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

    inline String encryptionTypeToString(wifi_auth_mode_t encType) {
        switch(encType) {
            case WIFI_AUTH_OPEN:
                return "Open";
            case WIFI_AUTH_WEP:
                return "WEP";
            case WIFI_AUTH_WPA_PSK:
                return "WPA-PSK";
            case WIFI_AUTH_WPA2_PSK:
                return "WPA2-PSK";
            case WIFI_AUTH_WPA_WPA2_PSK:
                return "WPA/WPA2";
            case WIFI_AUTH_WPA2_ENTERPRISE:
                return "WPA2-ENT";
            default:
                return "Unknown";
        }
    }
}
