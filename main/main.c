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
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "station.h"
#include "seven_seg.h"
#include "gpio.h"
#include "time.h"
#include "sound.h"
#include "tcp_server.h"
#include "lwip/sockets.h"

static const char *TAG = "main";

// HTTP Client logic to get current time
esp_err_t _http_event_handle(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            char buffer[128];
            esp_http_client_read(evt->client, buffer, evt->data_len);
            sscanf(buffer, "\"%d\"", &epoch_time);

            // start timer once epoch time is loaded
            time_start_ticking();

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


// 7 segment display config
seven_seg_t seven_seg = {
    .seg_pins = {17, 15, 18, 5, 19, 22, 16},
    .cathode_pins = {12, 33, 27},
    .colon_pin = 21
};

void seven_seg_mux_task() {

    for (int i = 0; ; i++) {
        uint32_t display_number = time_get_minutes();
        if (i % 2 == 0) {
            // tens
            seven_seg_set_number(&seven_seg, (display_number % 100) / 10);
            seven_seg_set_cathode(&seven_seg, SEVEN_SEGMENT_HOURS);
        } else {
            // ones
            seven_seg_set_number(&seven_seg, display_number % 10);
            seven_seg_set_cathode(&seven_seg, SEVEN_SEGMENT_MINS);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}



void app_main(void) {
    // initialize nvs
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    wifi_init_sta();

    seven_seg_init(&seven_seg);

    // gpio peripherals (only the LED)
    uint8_t peripherals[] = {2};

    gpio_peripheral_init(peripherals, 1);

    time_init();

    sound_init(23, 32, 13);

    ESP_ERROR_CHECK(esp_netif_init());

    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);

    ESP_LOGI(TAG, "Finished Initializing, fetching current time");

    esp_http_client_config_t config = {
    .url = "http://showcase.api.linx.twenty57.net/UnixTime/tounix?date=now",
    .event_handler = _http_event_handle,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
    }
    esp_http_client_cleanup(client);

    ESP_LOGI(TAG, "Finished fetching current time");

    xTaskCreate(seven_seg_mux_task, "seven_seg_mux_task", 2048, NULL, 3, NULL);

}
