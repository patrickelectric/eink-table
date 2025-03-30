#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>
#include <time.h>
#include <vector>

LV_FONT_DECLARE(font_awesome);

#include "peripherals/lvgl_driver.h"
#include "utils/server.h"
#include "widgets/calendar_widget.h"
#include "widgets/date_widget.h"
#include "widgets/network_info_widget.h"
#include "widgets/rate_chart_widget.h"
#include "widgets/rate_value_widget.h"
#include "widgets/weather_widget.h"
#include "widgets/pokemon_widget.h"

#include "sprites/all.h"
#include "utils/wifi_manager.h"

const std::vector<WiFiCredentials> wifi_networks = {
    {"ssid", "password"},
};

auto rate_chart = RateChartWidget();
auto rate_value = RateValueWidget();
auto date = DateWidget();
auto calendar = CalendarWidget();
auto network_info = NetworkInfoWidget();
auto weather = WeatherWidget();
auto pokemon = PokemonWidget();

LV_IMG_DECLARE(bulbasaur);
void lvgl_app_main(void)
{
    rate_chart.update();
    rate_value.update();
    date.update();
    calendar.update();
    network_info.update();
    weather.update();
    pokemon.update();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting...");

    // Initialize with vector of networks
    WiFiManager::self()->begin(wifi_networks);

    while (!WiFiManager::self()->isConnected()) {
        delay(100);
    }

    Date::init();
    WebServerManager::self()->begin();

    lv_init();

    lv_theme_t* th = lv_theme_default_init(
        lv_disp_get_default(),
        lv_color_make(0xff, 0, 0), // primary (highlight) color
        lv_color_white(), false, lv_font_default());
    lv_disp_set_theme(lv_disp_get_default(), th);

    lvgl_display_init();
    lvgl_app_main();
}

void loop()
{
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
    Serial.printf("CPU Temp: %.2f °C\n", temperatureRead());
    Serial.printf("Uptime: %lu ms\n", millis());
    Serial.printf("Time: %s\n", tm_to_string(Date::today()).c_str());
}