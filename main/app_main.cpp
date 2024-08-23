/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "i2c_oled.h"
#include "cbspI2C.h"
#include "cBMP280.h"
#include "cSMP3011.h"

static const char *TAG = "example";

#define I2C_BUS_PORT  0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_PIN_NUM_SDA           GPIO_NUM_5
#define EXAMPLE_PIN_NUM_SCL           GPIO_NUM_4

void example_lvgl_demo_ui();
cbspI2C I2CChannel1;
cBMP280 BMP280;
cSMP3011 SMP3011;

extern "C"
void app_main()
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags
        {
            .enable_internal_pullup = true,
        }
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);
    BMP280.init(I2CChannel1);
    SMP3011.init(I2CChannel1);

    i2c_oled_start(i2c_bus);

    //ESP_LOGI(TAG, "Display LVGL Scroll Text");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    //if (lvgl_port_lock(0)) {
    //    example_lvgl_demo_ui();
    //    // Release the mutex
    //    lvgl_port_unlock();
    //}
    

    lvgl_port_lock(0);
    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);
        
    lv_obj_t *labelPressure = lv_label_create(scr);
    lv_label_set_long_mode(labelPressure, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelPressure, "");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(labelPressure, 128);
    lv_obj_align(labelPressure, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *labelTemperature = lv_label_create(scr);
    lv_label_set_long_mode(labelTemperature, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelTemperature, "");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(labelTemperature, 128);
    lv_obj_align(labelTemperature, LV_ALIGN_TOP_MID, 0, 16);

    lv_obj_t *labelPressureAtm = lv_label_create(scr);
    lv_label_set_long_mode(labelPressureAtm, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelPressureAtm, "");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(labelPressureAtm, 128);
    lv_obj_align(labelPressureAtm, LV_ALIGN_TOP_MID, 0, 32);

    lv_obj_t *labelTemperatureAtm = lv_label_create(scr);
    lv_label_set_long_mode(labelTemperatureAtm, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelTemperatureAtm, "");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(labelTemperatureAtm, 128);
    lv_obj_align(labelTemperatureAtm, LV_ALIGN_TOP_MID, 0, 48);    

    lvgl_port_unlock();

    while(1)
    {
        BMP280.poll();
        SMP3011.poll();

        lvgl_port_lock(0);
        lv_label_set_text_fmt(labelPressure, "%6.0fPa", SMP3011.getPressure());
        lv_label_set_text_fmt(labelTemperature, "%6.2fC", SMP3011.getTemperature());
        lv_label_set_text_fmt(labelPressureAtm, "%6.0fPa", BMP280.getPressure());
        lv_label_set_text_fmt(labelTemperatureAtm, "%6.2fC", BMP280.getTemperature());
        lvgl_port_unlock();

        vTaskDelay(20/portTICK_PERIOD_MS);
    }

    
}
