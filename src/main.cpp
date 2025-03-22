#include <Arduino.h>
#include <lvgl.h>
#include <time.h> 
#include <vector>
#include <WiFi.h>

#include "date.h"
#include "rates.h"
#include "peripherals/lvgl_driver.h"

const char* ssid = "ssid";
const char* password = "password";

void create_chart(std::vector<float> points) {
    auto chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, LV_HOR_RES_MAX/3, LV_VER_RES_MAX/2);
    lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    const auto min_value = *std::min_element(points.begin(), points.end());
    const auto max_value = *std::max_element(points.begin(), points.end());

    auto series = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, min_value * 100, max_value * 100);
    lv_chart_set_point_count(chart, points.size());
    for(const auto point: points) {
        lv_chart_set_next_value(chart, series, static_cast<int32_t>(point * 100));
    }

    lv_chart_refresh(chart);
}

void dolar_value(float value)
{
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, cont_w, cont_h);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *label = lv_label_create(cont);
    char buffer[10];
    sprintf(buffer, "$ %.2f", value);
    lv_label_set_text(label, buffer);
    lv_obj_center(label);
}

void date(tm date)
{
    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, cont_w, cont_h);
    lv_obj_align(cont, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t *label = lv_label_create(cont);
    char buffer[15];
    sprintf(buffer, "%d/%d/%d", date.tm_mday, date.tm_mon, date.tm_year + 1900);
    lv_label_set_text(label, buffer);
    lv_obj_center(label);
}

void calendar(tm date)
{
    const int year = date.tm_year + 1900;
    const int month = date.tm_mon;
    const int day = date.tm_mday;

    uint16_t scr_w = LV_HOR_RES_MAX;
    uint16_t scr_h = LV_VER_RES_MAX;
    uint16_t cont_w = scr_w / 3;
    uint16_t cont_h = scr_h / 2;

    lv_obj_t* calendar = lv_calendar_create(lv_scr_act());
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

    auto status_to_text = [](wl_status_t status) -> String {
        switch (status) {
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
    };

    std::vector<String> netInfo;
    netInfo.push_back("SSID: " + WiFi.SSID());
    netInfo.push_back("IP: " + WiFi.localIP().toString());
    netInfo.push_back("RSSI: " + String(WiFi.RSSI()) + " dBm");
    netInfo.push_back("Status: " + status_to_text(WiFi.status()));
    for (const auto& info : netInfo) {
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, info.c_str());
    }
}

LV_IMG_DECLARE(bulbasaur);
void lvgl_app_main (void)
{
    create_chart(Rates::last10FridaysRates());
    dolar_value(Rates::currentRate());
    date(Date::today());
    calendar(Date::today());
    network_info();

    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &bulbasaur);
    lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, 0);

}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.printf("Connecting to %s (%d)\n\r", ssid, WiFi.status());
    }

    Date::init();

    lv_init();

    lv_theme_t *th = lv_theme_default_init(
        lv_disp_get_default(),
        lv_color_make(0xff, 0, 0),    // primary (highlight) color
        lv_color_white(),
        false,
        lv_font_default()
    );
    lv_disp_set_theme(lv_disp_get_default(), th);

    lvgl_display_init();

    lvgl_app_main();
}

void loop() {
    lv_timer_handler();
    delay(5);

    auto usedSRAM = ESP.getFreeHeap();
    auto usedPSRAM = ESP.getFreePsram();
    /*
    Serial.println("");
    Serial.print("**SRAM allocated = ");
    Serial.println(usedSRAM);
    Serial.print("**PSRAM allocated = ");
    Serial.println(usedPSRAM);
    */
}