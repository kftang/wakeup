
void time_tick(void *);
void time_init();
int time_get_minutes();
int time_get_hours();
void time_start_ticking();

extern volatile uint32_t epoch_time;
extern volatile uint32_t alarm_time;
