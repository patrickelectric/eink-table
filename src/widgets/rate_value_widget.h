#pragma once

#include "utils/date.h"
#include "utils/rates.h"
#include <lvgl.h>

class RateValueWidget {
    float value;
    tm last_update;

public:
    RateValueWidget()
        : value(0.0f)
        , last_update()
    {
    }

    static void update_value(float value, float previous_value)
    {
        uint16_t scr_w = LV_HOR_RES_MAX;
        uint16_t scr_h = LV_VER_RES_MAX;
        uint16_t cont_w = scr_w / 3;
        uint16_t cont_h = scr_h / 2;

        static lv_obj_t* cont = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cont, cont_w, cont_h);
        lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);

        static lv_obj_t* label = lv_label_create(cont);
        lv_obj_set_style_text_font(label, &font_awesome, LV_PART_MAIN);
        String icon = previous_value > value ? "\ue097" : "\ue098";
        char buffer[100];
        sprintf(buffer, "%s $ %.2f", icon, value);
        lv_label_set_text(label, buffer);
        lv_obj_center(label);
    }

    void update()
    {
        debug("Updating...");
        auto today = Date::today();
        if (today == last_update && value != 0.0) {
            return;
        }
        debug("today: %d/%d/%d, last_update: %d/%d/%d", today.tm_mday, today.tm_mon,
            today.tm_year + 1900, last_update.tm_mday, last_update.tm_mon,
            last_update.tm_year + 1900);
        debug("value: %.2f", value);

        last_update = today;
        value = Rates::currentRate();
        auto last_30_days_value = Rates::rate30DaysAgo();
        RateValueWidget::update_value(value, last_30_days_value);
    }
};
