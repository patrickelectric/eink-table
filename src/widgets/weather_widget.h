#pragma once

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <lvgl.h>
#include <vector>

#include "utils/date.h"
#include "utils/debug.h"

class WeatherWidget {
    tm last_update;

public:
    WeatherWidget()
        : last_update()
    {
    }

    static bool update_info()
    {
        uint16_t scr_w = LV_HOR_RES_MAX;
        uint16_t scr_h = LV_VER_RES_MAX;
        uint16_t cont_w = scr_w / 3;
        uint16_t cont_h = scr_h / 2;

        static lv_obj_t* cont = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cont, cont_w, cont_h);
        lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER);

        HTTPClient http;
        http.begin("https://wttr.in/Florianopolis?format=j1");
        int code = http.GET();

        auto sucess = false;
        if (code == 200) {
            String payload = http.getString();
            JsonDocument doc;
            deserializeJson(doc, payload);

            const char* tempC = doc["current_condition"][0]["temp_C"];
            const char* desc = doc["current_condition"][0]["weatherDesc"][0]["value"];

            String icon_txt;
            String desc_str(desc);
            desc_str.toLowerCase();

            if (desc_str.indexOf("partly") >= 0 && desc_str.indexOf("cloud") >= 0) {
                icon_txt = "\uf6c4";
            } else if (desc_str.indexOf("sun") >= 0) {
                icon_txt = "\uf185";
            } else if (desc_str.indexOf("cloud") >= 0) {
                icon_txt = "\uf0c2";
            } else if (desc_str.indexOf("rain") >= 0) {
                icon_txt = "\uf73d";
            } else if (desc_str.indexOf("storm") >= 0 || desc_str.indexOf("thunder") >= 0) {
                icon_txt = "\uf0e7";
            } else if (desc_str.indexOf("snow") >= 0) {
                icon_txt = "\uf2dc";
            } else if (desc_str.indexOf("fog") >= 0 || desc_str.indexOf("mist") >= 0) {
                icon_txt = "\uf75f";
            } else {
                icon_txt = String("\uf128 ") + desc_str;
            }

            static lv_obj_t* icon = lv_label_create(cont);
            lv_label_set_text(icon, icon_txt.c_str());
            lv_obj_set_style_text_font(icon, &font_awesome, 0);

            static lv_obj_t* label = lv_label_create(cont);
            String weather_info = String(desc) + " " + tempC + "°C";
            lv_label_set_text(label, weather_info.c_str());
            sucess = true;
        } else {
            static lv_obj_t* label = lv_label_create(cont);
            lv_label_set_text(label, "Weather fetch error");
        }
        http.end();
        return sucess;
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

        if (WeatherWidget::update_info()) {
            last_update = today;
        }
    }
};
