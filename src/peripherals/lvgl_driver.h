#ifndef LVGL_DRIVER_H
#define LVGL_DRIVER_H

#include <lvgl.h>

#define LV_VER_RES_MAX 384
#define LV_HOR_RES_MAX 640
#define LVGL_TICK_PERIOD_MS 500

// Function to initialize the display driver
void lvgl_display_init();

// Function to flush LVGL's buffer to the ePaper display
void lvgl_display_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);

// Function to control partial/full refresh
void lvgl_update_display(bool full_refresh);

#endif  // LVGL_DRIVER_H