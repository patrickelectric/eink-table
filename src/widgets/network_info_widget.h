#pragma once

#include <WiFi.h>
#include <lvgl.h>
#include <vector>

#include "utils/date.h"
#include "utils/debug.h"
#include "utils/network.h"
class NetworkInfoWidget {
    tm last_update;

public:
    NetworkInfoWidget()
        : last_update()
    {
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

        static lv_obj_t* ssid_label = lv_label_create(cont);
        lv_label_set_text(ssid_label, ("SSID: " + WiFi.SSID()).c_str());
        static lv_obj_t* ip_label = lv_label_create(cont);
        lv_label_set_text(ip_label, ("IP: " + WiFi.localIP().toString()).c_str());
        static lv_obj_t* rssi_label = lv_label_create(cont);
        lv_label_set_text(rssi_label, ("RSSI: " + String(WiFi.RSSI()) + " dBm").c_str());
        static lv_obj_t* status_label = lv_label_create(cont);
        lv_label_set_text(status_label, ("Status: " + Network::wifiStatusToString(WiFi.status())).c_str());
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
