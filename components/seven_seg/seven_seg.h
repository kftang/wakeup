#include <stdint.h>

struct seven_seg {
    uint8_t seg_pins[7];
    uint8_t cathode_pins[3];
};
typedef struct seven_seg seven_seg_t;

enum seven_seg_cathode {
    SEVEN_SEGMENT_HOURS,
    SEVEN_SEGMENT_COLON,
    SEVEN_SEGMENT_MINS
};
typedef enum seven_seg_cathode seven_seg_cathode_t;

void seven_seg_set_cathode(seven_seg_t *, seven_seg_cathode_t);
void seven_seg_set_number(seven_seg_t *, uint8_t);
void seven_seg_init(seven_seg_t *);