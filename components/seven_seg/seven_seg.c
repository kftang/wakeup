#include "driver/gpio.h"
#include "esp_log.h"
#include <stdint.h>

#include "seven_seg.h"

static const char *TAG = "seven_seg";

static const uint8_t numbers[] = {
        0x3f, // 0b00111111,    // 0
        0x06, // 0b00000110,    // 1
        0x5b, // 0b01011011,    // 2
        0x4f, // 0b01001111,    // 3
        0x66, // 0b01100110,    // 4
        0x6d, // 0b01101101,    // 5
        0x7d, // 0b01111101,    // 6
        0x07, // 0b00000111,    // 7
        0x7f, // 0b01111111,    // 8
        0x6f, // 0b01101111,    // 9
};

void seven_seg_set_cathode(seven_seg_t * seven_seg, seven_seg_cathode_t cathode) {
    // set all cathodes to low first
    for (int i = 0; i < 3; i++) {
        gpio_set_level(seven_seg->cathode_pins[i], 1);
    }

    // control colon with cathode
    if (cathode == SEVEN_SEGMENT_COLON) {
        gpio_set_level(seven_seg->colon_pin, 0);
    }

    // set specified cathode high
    gpio_set_level(seven_seg->cathode_pins[cathode], 0);
}

void seven_seg_set_number(seven_seg_t * seven_seg, uint8_t num) {
    // set all segments to low first
    for (int i = 0; i < 7; i++) {
        gpio_set_level(seven_seg->seg_pins[i], 1);
    }

    // set segments
    for (int i = 0; i < 7; i++) {
        uint8_t segment_mask = 1 << i;
        uint8_t pin_value = !(numbers[num] & segment_mask);

        if (!pin_value) {
            gpio_set_level(seven_seg->seg_pins[i], pin_value);
        }
    }
}

void seven_seg_init(seven_seg_t * seven_seg) {
    // create bitmask for all output pins
    uint64_t output_pins_bitmask = 0;
    for (int i = 0; i < 7; i++) {
        output_pins_bitmask |= (1uLL << seven_seg->seg_pins[i]);
    }
    for (int i = 0; i < 3; i++) {
        output_pins_bitmask |= (1uLL << seven_seg->cathode_pins[i]);
    }
    output_pins_bitmask |= (1uLL << seven_seg->colon_pin);

    // configure gpio pins for output pins
    gpio_config_t io_config = {
        .pin_bit_mask = output_pins_bitmask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_config));

    // set all segments off (active low)
    for (int i = 0; i < 7; i++) {
        ESP_ERROR_CHECK(gpio_set_level(seven_seg->seg_pins[i], 1));
    }
    ESP_ERROR_CHECK(gpio_set_level(seven_seg->colon_pin, 1));

    // set all cathodes off (active high)
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(gpio_set_level(seven_seg->cathode_pins[i], 0));
    }
}
