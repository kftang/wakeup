#include "driver/timer.h"
#include "freertos/FreeRTOS.h"

#include "time.h"
#include "sound.h"

volatile uint32_t epoch_time = 0;
volatile uint32_t alarm_time = 0;

void IRAM_ATTR timer_tg0_isr(void * args) {
	//Reset irq and set for next time
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;

    epoch_time++;
    if (epoch_time == alarm_time) {
        sound_play();
    }
}

int time_get_hours() {
    return epoch_time % 86400 / 3600;
}

int time_get_minutes() {
    return epoch_time % 3600 / 60;
}

void time_init() {
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 8000
    };

    // init timerg0 hwtimer0 counting every second
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 10000);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_tg0_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
}

void time_start_ticking() {
    timer_start(TIMER_GROUP_0, TIMER_0);
}
