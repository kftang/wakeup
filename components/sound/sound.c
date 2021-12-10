#include <driver/dac.h>

void sound_play() {
    dac_cw_generator_enable();
}

void sound_stop() {
    dac_cw_generator_disable();
}

void sound_init() {
    dac_output_enable(DAC_CHANNEL_1);
    dac_cw_config_t cw_config = {
        .en_ch = DAC_CHANNEL_1,
        .scale = DAC_CW_SCALE_1,
        .phase = DAC_CW_PHASE_0,
        .freq = 1000,
    };
    dac_cw_generator_config(&cw_config);
}
