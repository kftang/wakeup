#include <driver/dac.h>
#include <driver/gpio.h>

void sound_play() {
    dac_cw_generator_enable();
}

void sound_stop() {
    dac_cw_generator_disable();
}

void sound_init(uint8_t gain0_pin, uint8_t gain1_pin, uint8_t shutdown_pin) {
    dac_output_enable(DAC_CHANNEL_1);
    dac_cw_config_t cw_config = {
        .en_ch = DAC_CHANNEL_1,
        .scale = DAC_CW_SCALE_1,
        .phase = DAC_CW_PHASE_0,
        .freq = 1000,
    };
    dac_cw_generator_config(&cw_config);

    // create bitmask for output pins
    uint64_t output_pins_bitmask = 0;
    output_pins_bitmask |= (1uLL << gain0_pin);
    output_pins_bitmask |= (1uLL << gain1_pin);
    output_pins_bitmask |= (1uLL << shutdown_pin);

    // configure gpio pins for output pins
    gpio_config_t io_config = {
        .pin_bit_mask = output_pins_bitmask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_config);

    gpio_set_level(gain0_pin, 0);
    gpio_set_level(gain1_pin, 0);
    gpio_set_level(shutdown_pin, 1);
}
