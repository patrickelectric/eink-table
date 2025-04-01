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
    delay(20000);

    auto freeSRAM = ESP.getFreeHeap();
    auto freePSRAM = ESP.getFreePsram();

    debug("Free heap: %u bytes", ESP.getFreeHeap());
    debug("Free PSRAM: %u bytes", ESP.getFreePsram());
    debug("Heap size: %u bytes", ESP.getHeapSize());
    debug("Min free heap: %u bytes", ESP.getMinFreeHeap());
    debug("Max alloc heap: %u bytes", ESP.getMaxAllocHeap());
    debug("CPU Temp: %.2f °C", temperatureRead());
    debug("Uptime: %lu ms", millis());
    debug("Time: %s", tm_to_string(Date::today()).c_str());
}