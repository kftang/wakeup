#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg) {
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

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
        output_pins_bitmask |= (1uLL << peripheral_pins[i]);
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

    // turn off pin initially
    for (int i = 0; i < num_peripherals; i++) {
        gpio_set_level(peripheral_pins[i], 0);
    }
}

void gpio_queue_init() {
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //install gpio isr service
    gpio_install_isr_service(0);
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
}

void gpio_switch_init(uint8_t switch_pin, void (*int_handler)(void *)) {
    // configure gpio pins for input pin
    gpio_config_t io_config = {
        .pin_bit_mask = 1uLL << switch_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_config);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(switch_pin, gpio_isr_handler, (void *) switch_pin);
}
