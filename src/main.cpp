#include <Arduino.h>
#include <lvgl.h>
#include "peripherals/lvgl_driver.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    lv_init();
    lvgl_display_init();

    lv_obj_t *box = lv_obj_create(lv_scr_act());
    lv_obj_set_size(box, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_align(box, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(box, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(box, lv_color_black(), 0);
    lv_obj_set_style_border_width(box, 2, 0);

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, World!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);

    lv_obj_t *rect = lv_obj_create(lv_scr_act());
    lv_obj_set_size(rect, 100, 50);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(rect, lv_color_black(), LV_PART_MAIN);

    // Draw line crossing the diagonal (from top right to bottom left)
    lv_obj_t *line = lv_line_create(lv_scr_act());
    lv_point_t points[] = { {0, 0}, {100, 100} };
    lv_line_set_points(line, points, 2);
    lv_obj_set_style_line_color(line, lv_color_black(), 0);
    lv_obj_set_style_line_width(line, 2, 0);

    lv_obj_t *chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 150, 100);
    lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
    for(uint8_t i = 0; i < 10; i++){
        lv_chart_set_next_value(chart, ser, random(0, 100));
    }
}

void loop() {
    lv_timer_handler();
    delay(5);

    auto usedSRAM = ESP.getFreeHeap();
    auto usedPSRAM = ESP.getFreePsram();
    Serial.println("");
    Serial.print("**SRAM allocated = ");
    Serial.println(usedSRAM);
    Serial.print("**PSRAM allocated = ");
    Serial.println(usedPSRAM);
}