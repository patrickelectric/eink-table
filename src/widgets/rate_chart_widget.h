#pragma once

#include "utils/date.h"
#include "utils/debug.h"
#include "utils/rates.h"
#include <algorithm>
#include <lvgl.h>
#include <vector>

class RateChartWidget {
    std::vector<float> points;
    tm last_update;

public:
    RateChartWidget()
        : points({})
        , last_update()
    {
    }

    static void update_chart(std::vector<float> points)
    {
        debug("Updating!");
        static auto chart = lv_chart_create(lv_scr_act());
        lv_obj_set_size(chart, LV_HOR_RES_MAX / 3, LV_VER_RES_MAX / 2);
        lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

        const auto min_value = *std::min_element(points.begin(), points.end());
        const auto max_value = *std::max_element(points.begin(), points.end());

        static auto series = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, min_value * 100,
            max_value * 100);
        lv_chart_set_point_count(chart, points.size());
        for (const auto point : points) {
            lv_chart_set_next_value(chart, series, static_cast<int32_t>(point * 100));
        }

        lv_chart_refresh(chart);
    }

    void update()
    {
        debug("Updating...");
        auto today = Date::today();
        if (today == last_update && points.size() != 0) {
            return;
        }
        debug("today: %d/%d/%d, last_update: %d/%d/%d", today.tm_mday, today.tm_mon,
            today.tm_year + 1900, last_update.tm_mday, last_update.tm_mon,
            last_update.tm_year + 1900);
        debug("points.size(): %d", points.size());
        points = Rates::last10FridaysRates();
        if (points.size() != 0) {
            RateChartWidget::update_chart(points);
            last_update = today;
        }
    }
};
