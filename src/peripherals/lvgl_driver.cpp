#include "lvgl_driver.h"
#include <lvgl.h>
#include <GxEPD2_3C.h>

#define EPD_SCK_PIN  36
#define EPD_MOSI_PIN 35
#define EPD_CS_PIN   34
#define EPD_RST_PIN  1
#define EPD_DC_PIN   0
#define EPD_BUSY_PIN 2

GxEPD2_3C<GxEPD2_750c, GxEPD2_750c::HEIGHT> display(
    GxEPD2_750c(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

static lv_disp_draw_buf_t draw_buf;

void clear_display() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.display();
}

void ssd1677_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    Serial.println("ssd1677_flush triggered");
    display.setPartialWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);

    Serial.println("display.firstPage()");
    Serial.printf("area->x1: %d\n\r", area->x1);
    Serial.printf("area->y1: %d\n\r", area->y1);
    Serial.printf("area->x2: %d\n\r", area->x2);
    Serial.printf("area->y2: %d\n\r", area->y2);
    display.firstPage();
    do {
        for (int y = area->y1; y <= area->y2; y++) {
            for (int x = area->x1; x <= area->x2; x++) {
                lv_color_t pixel = color_p[(y - area->y1) * (area->x2 - area->x1 + 1) + (x - area->x1)];
                display.drawPixel(x, y, pixel.full == 0 ? GxEPD_BLACK : GxEPD_WHITE);
            }
        }
    } while (display.nextPage());

    lv_disp_flush_ready(disp);
}

void increase_lvgl_tick(void *arg) {
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

void lvgl_display_init() {
    SPI.begin(EPD_SCK_PIN, -1, EPD_MOSI_PIN);
    display.init(115200, true, 2, false);
    clear_display();

    lv_color_t *buf = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LV_HOR_RES_MAX * 100);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = display.width();
    disp_drv.ver_res = display.height();
    disp_drv.flush_cb = ssd1677_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
}