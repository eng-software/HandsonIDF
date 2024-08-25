#include <cstdio>
#include "i2c_oled.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"

extern void example_lvgl_demo_ui(lv_disp_t *disp);

extern "C"
void app_main()
{        
    i2c_oled_init();

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0)) {
        example_lvgl_demo_ui(nullptr);
        // Release the mutex
        lvgl_port_unlock();
    }
}