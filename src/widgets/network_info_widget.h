#pragma once

#include <WiFi.h>
#include <lvgl.h>
#include <vector>

#include "utils/date.h"
#include "utils/debug.h"

class NetworkInfoWidget {
    tm last_update;

public:
    NetworkInfoWidget()
        : last_update()
    {
    }

    static String wifi_status_to_string()
    {
        switch (WiFi.status()) {
        case WL_NO_SHIELD:
            return "No Shield";
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_NO_SSID_AVAIL:
            return "No SSID";
        case WL_SCAN_COMPLETED:
            return "Scan Completed";
        case WL_CONNECTED:
            return "Connected";
        case WL_CONNECT_FAILED:
            return "Failed";
        case WL_CONNECTION_LOST:
            return "Lost";
        case WL_DISCONNECTED:
            return "Disconnected";
        default:
            return "Unknown";
        }
    }

    static void update_network_info()
    {
        // Define container size as 1/3 of screen width and half of screen height.
        uint16_t scr_w = LV_HOR_RES_MAX;
        uint16_t scr_h = LV_VER_RES_MAX;
        uint16_t cont_w = scr_w / 3;
        uint16_t cont_h = scr_h / 2;

        // Create container at the bottom-left corner.
        static lv_obj_t* cont = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cont, cont_w, cont_h);
        lv_obj_align(cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER);

        std::vector<String> netInfo;
        netInfo.push_back("SSID: " + WiFi.SSID());
        netInfo.push_back("IP: " + WiFi.localIP().toString());
        netInfo.push_back("RSSI: " + String(WiFi.RSSI()) + " dBm");
        netInfo.push_back("Status: " + wifi_status_to_string());
        for (const auto& info : netInfo) {
            lv_obj_t* label = lv_label_create(cont);
            lv_label_set_text(label, info.c_str());
        }
    }

    void update()
    {
        debug("Updating...");
        auto today = Date::today();
        if (today == last_update) {
            return;
        }
        debug("today: %d/%d/%d, last_update: %d/%d/%d", today.tm_mday, today.tm_mon,
            today.tm_year + 1900, last_update.tm_mday, last_update.tm_mon,
            last_update.tm_year + 1900);
        last_update = today;
        NetworkInfoWidget::update_network_info();
    }
};
