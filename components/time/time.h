
void time_tick(void * args);
void time_init();
int time_get_minutes();
int time_get_hours();
void time_start_ticking();

extern volatile uint32_t epoch_time;
