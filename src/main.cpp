#include <Arduino.h>
#include <lvgl.h>
#include "peripherals/lvgl_driver.h"

void create_chart() {
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, LV_HOR_RES_MAX/3, LV_VER_RES_MAX/2);
    lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    lv_chart_set_point_count(chart, 20);
    // lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    for(i = 0; i < 20; i++) {
        lv_chart_set_next_value(chart, ser1, (int32_t)lv_rand(0, 50));
    }

    lv_chart_refresh(chart);
}

void dolar_value()
{
    // Define container size as 1/3 of screen width and height.
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    // Create container in the top right corner.
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, cont_w, cont_h);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);

    // Create label and center it within the container.
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "$ 5.67");
    lv_obj_center(label);
}

void date()
{
    // Define container size as 1/3 of screen width and height.
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    // Create container in the top right corner.
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, cont_w, cont_h);
    lv_obj_align(cont, LV_ALIGN_TOP_RIGHT, 0, 0);

    // Create label and center it within the container.
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "20 Mar 2025");
    lv_obj_center(label);
}

void calendar()
{
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    lv_obj_t* calendar = lv_calendar_create(lv_scr_act());
    static lv_style_t style_sel;
    lv_style_init(&style_sel);
    lv_style_set_bg_color(&style_sel, lv_color_black());
    //lv_obj_add_style(calendar, &style_sel, LV_CALENDAR_PART_DATE | LV_STATE_CHECKED);
    //lv_obj_set_style_bg_color(cal, lv_color_black(), LV_CALENDAR_PART_TODAY);


    lv_obj_set_size(calendar, cont_w, cont_h);
    lv_obj_align(calendar, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_calendar_set_today_date(calendar, 2025, 03, 20);
    lv_calendar_set_showed_date(calendar, 2025, 03);
    //lv_calendar_set_pressed_date(calendar, 2025, 03, 20);
    static lv_calendar_date_t highlighted_days[0];       /*Only its pointer will be saved so should be static*/
    highlighted_days[0].year = 2025;
    highlighted_days[0].month = 3;
    highlighted_days[0].day = 20;
    lv_calendar_set_highlighted_dates(calendar, highlighted_days, 3);
    lv_obj_set_style_bg_color(calendar, lv_color_black(), LV_STATE_CHECKED);
    lv_obj_set_style_text_color(calendar, lv_color_white(), LV_STATE_CHECKED);

    /*
    static lv_style_t style_sel;
    lv_style_init(&style_sel);
    lv_style_set_bg_color(&style_sel, lv_color_black());
    lv_style_set_text_color(&style_sel, lv_color_white());
    lv_calendar_(calendar, LV_CALENDAR_STYLE_SELECTED_DATE, &style_sel);
    */
}

void network_info()
{
    // Define container size as 1/3 of screen width and half of screen height.
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    // Create container at the bottom-left corner.
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, cont_w, cont_h);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Create 5 labels vertically aligned inside the container.
    const char *texts[5] = {"20 Mar 2025", "IP: 192.168.1.2", "Status: Online", "SSID: HomeWiFi", "Signal: -67 dBm"};
    for (int i = 0; i < 5; i++) {
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, texts[i]);
    }
}

void lvgl_app_main (void)
{
    create_chart();
    dolar_value();
    date();
    calendar();
    network_info();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    lv_init();

    lv_theme_t *th = lv_theme_default_init(
        lv_disp_get_default(),
        lv_color_black(),    // primary (highlight) color
        lv_color_white(),
        false,
        lv_font_default()
    );
    lv_disp_set_theme(lv_disp_get_default(), th);

    lvgl_display_init();

    lvgl_app_main();


    lv_obj_t *chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 150, 100);
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
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