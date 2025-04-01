#pragma once

#include "utils/date.h"
#include "utils/debug.h"
#include <lvgl.h>

class DateWidget {
    tm last_update;

public:
    DateWidget()
        : last_update()
    {
    }

    static void update_date(tm date)
    {
        uint16_t scr_w = LV_HOR_RES_MAX;
        uint16_t scr_h = LV_VER_RES_MAX;
        uint16_t cont_w = scr_w / 3;
        uint16_t cont_h = scr_h / 2;

        static lv_obj_t* cont = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cont, cont_w, cont_h);
        lv_obj_align(cont, LV_ALIGN_TOP_RIGHT, 0, 0);

        static lv_obj_t* label = lv_label_create(cont);
        char buffer[15];
        sprintf(buffer, "%d/%d/%d", date.tm_mday, date.tm_mon + 1,
            date.tm_year + 1900);
        lv_label_set_text(label, buffer);
        lv_obj_center(label);
    }

    void update()
    {
        debug("Updating...");
        auto today = Date::today();
        if (today == last_update) {
            return;
        }
        debug("today: %d/%d/%d, last_update: %d/%d/%d", today.tm_mday,
            today.tm_mon + 1, today.tm_year + 1900, last_update.tm_mday,
            last_update.tm_mon + 1, last_update.tm_year + 1900);
        last_update = today;
        DateWidget::update_date(today);
    }
};
