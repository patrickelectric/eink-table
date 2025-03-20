#include <Arduino.h>
#include <lvgl.h>
#include "peripherals/lvgl_driver.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    lv_init();
    lvgl_display_init();

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, World!");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, -20);

    lv_obj_t *rect = lv_obj_create(lv_scr_act());
    lv_obj_set_size(rect, 100, 50);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, -10);
    lv_obj_set_style_bg_color(rect, lv_color_white(), LV_PART_MAIN);

    lv_obj_t * line = lv_line_create(lv_scr_act());
    lv_point_t points[] = { {0, 0}, {LV_HOR_RES_MAX, 0}};
    lv_line_set_points(line, points, 2);

    lv_obj_t * line2 = lv_line_create(lv_scr_act());
    lv_point_t points2[] = { {0, 0}, {0, LV_VER_RES_MAX}};
    lv_line_set_points(line2, points2, 2);

    lv_obj_t * line3 = lv_line_create(lv_scr_act());
    lv_point_t points3[] = { {LV_HOR_RES_MAX, 0}, {LV_HOR_RES_MAX, LV_VER_RES_MAX}};
    lv_line_set_points(line3, points3, 2);

    lv_obj_t * line4 = lv_line_create(lv_scr_act());
    lv_point_t points4[] = { {0, LV_VER_RES_MAX}, {LV_HOR_RES_MAX, LV_VER_RES_MAX}};
    lv_line_set_points(line4, points4, 2);
}

void loop() {
    lv_timer_handler();
    delay(5);
}