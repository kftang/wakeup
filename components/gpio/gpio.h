#include <stdint.h>

void gpio_peripheral_on();
void gpio_peripheral_off();
void gpio_peripheral_init();
void gpio_switch_init(uint8_t, void (*)(void *));