#pragma once

#include "utils/date.h"
#include "utils/debug.h"
#include <lvgl.h>

class CalendarWidget {
    tm last_update;

public:
    CalendarWidget()
        : last_update()
    {
    }

    static void update_calendar(tm date)
    {
        const int year = date.tm_year + 1900;
        const int month = date.tm_mon;
        const int day = date.tm_mday;

        uint16_t scr_w = LV_HOR_RES_MAX;
        uint16_t scr_h = LV_VER_RES_MAX;
        uint16_t cont_w = scr_w / 3;
        uint16_t cont_h = scr_h / 2;

        static lv_obj_t* calendar = lv_calendar_create(lv_scr_act());
        static lv_style_t style_sel;
        lv_style_init(&style_sel);
        lv_style_set_bg_color(&style_sel, lv_color_black());

        lv_obj_set_size(calendar, cont_w, cont_h);
        lv_obj_align(calendar, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_calendar_set_today_date(calendar, year, month, day);
        lv_calendar_set_showed_date(calendar, year, month);
        static lv_calendar_date_t highlighted_days[0];
        highlighted_days[0].year = year;
        highlighted_days[0].month = month;
        highlighted_days[0].day = day;
        lv_calendar_set_highlighted_dates(calendar, highlighted_days, 1);
        lv_obj_set_style_bg_color(calendar, lv_color_black(), LV_STATE_CHECKED);
        lv_obj_set_style_text_color(calendar, lv_color_white(), LV_STATE_CHECKED);
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
        CalendarWidget::update_calendar(today);
    }
};
