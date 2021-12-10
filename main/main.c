/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "station.h"
#include "seven_seg.h"
#include "gpio.h"

void stop_sw(void * params) {
    printf("stop switch");
}

void snooze_sw(void * params) {
    printf("stop switch");
}

void app_main(void)
{
    // initialize nvs
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    printf("Hello world!\n");
    wifi_init_sta();

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    // 7 segment display config
    seven_seg_t seven_seg = {
        .seg_pins = {17, 15, 18, 5, 19, 22, 16},
        .cathode_pins = {12, 33, 27}
    };

    seven_seg_init(&seven_seg);

    // gpio peripherals (only the LED)
    uint8_t peripherals[] = {2};

    gpio_peripheral_init(peripherals, 1);

    // stop switch
    gpio_switch_init(34, &stop_sw);

    // snooze switch
    gpio_switch_init(35, &snooze_sw);
}