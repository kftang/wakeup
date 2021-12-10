#include <stdint.h>

#include "driver/gpio.h"

void gpio_peripheral_on(uint8_t peripheral_pin) {
    gpio_set_level(peripheral_pin, 1);
}

void gpio_peripheral_off(uint8_t peripheral_pin) {
    gpio_set_level(peripheral_pin, 0);
}

void gpio_peripheral_init(uint8_t * peripheral_pins, uint8_t num_peripherals) {
    // create bitmask for all output pins
    uint64_t output_pins_bitmask = 0;
    for (int i = 0; i < num_peripherals; i++) {
        output_pins_bitmask |= (1 << peripheral_pins[i]);
    }

    // configure gpio pins for output pins
    gpio_config_t io_config = {
        .pin_bit_mask = output_pins_bitmask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_config);
}

void gpio_switch_init(uint8_t switch_pin, void (*int_handler)(void *)) {
    // configure gpio pins for input pin
    gpio_config_t io_config = {
        .pin_bit_mask = 1 << switch_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&io_config);
    gpio_isr_handler_add(switch_pin, int_handler, NULL);
}