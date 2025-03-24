#include <Arduino.h>
#include <lvgl.h>
#include <time.h>
#include <vector>
#include <WiFi.h>

LV_FONT_DECLARE(font_awesome);

#include "peripherals/lvgl_driver.h"

#include "date.h"
// #include "rates.h"
#include "calendar_widget.h"
#include "date_widget.h"
#include "rate_chart_widget.h"
#include "rate_value_widget.h"
#include "network_info_widget.h"
#include "weather_widget.h"

#include "sprites/all.h"

const char* ssid = "ssid";
const char* password = "password";

auto rate_chart = RateChartWidget();
auto rate_value = RateValueWidget();
auto date = DateWidget();
auto calendar = CalendarWidget();
auto network_info = NetworkInfoWidget();
auto weather = WeatherWidget();
LV_IMG_DECLARE(bulbasaur);
void lvgl_app_main (void)
{
    rate_chart.update();
    rate_value.update();
    date.update();
    calendar.update();
    network_info.update();
    weather.update();

    static lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, all_sprites()[random(0, 151)]);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
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
    lvgl_app_main();
    lv_timer_handler();
    _lv_disp_refr_timer(NULL);
    delay(5000);

    auto freeSRAM = ESP.getFreeHeap();
    auto freePSRAM = ESP.getFreePsram();
    Serial.println("");
    Serial.print("**SRAM free = ");
    Serial.println(freeSRAM);
    Serial.print("**PSRAM free = ");
    Serial.println(freePSRAM);

    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Heap size: %u bytes\n", ESP.getHeapSize());
    Serial.printf("Min free heap: %u bytes\n", ESP.getMinFreeHeap());
    Serial.printf("Max alloc heap: %u bytes\n", ESP.getMaxAllocHeap());
    Serial.printf("CPU Temp: %u °C\n", temperatureRead());
    Serial.printf("Uptime: %lu ms\n", millis());
}