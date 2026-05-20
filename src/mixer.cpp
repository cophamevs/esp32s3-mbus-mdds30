#include "mixer.h"

static int8_t clamp8(int16_t v, int8_t lo, int8_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (int8_t)v;
}

int8_t mixer_map_channel(uint16_t raw,
                          uint16_t center,
                          uint16_t dead_zone,
                          uint16_t raw_min,
                          uint16_t raw_max,
                          int8_t   speed_max) {
    uint16_t lo = center - dead_zone;
    uint16_t hi = center + dead_zone;

    if (raw >= lo && raw <= hi) return 0;

    if (raw > hi) {
        uint16_t span = raw_max - hi;
        uint16_t pos  = raw - hi;
        return clamp8((int16_t)((int32_t)pos * speed_max / span), 0, speed_max);
    } else {
        uint16_t span = lo - raw_min;
        uint16_t pos  = lo - raw;
        return clamp8(-(int16_t)((int32_t)pos * speed_max / span), -speed_max, 0);
    }
}

void mixer_compute(int8_t  throttle,
                   int8_t  steer,
                   int8_t  speed_max,
                   int8_t* left,
                   int8_t* right) {
    *left  = clamp8((int16_t)throttle + steer, -speed_max, speed_max);
    *right = clamp8((int16_t)throttle - steer, -speed_max, speed_max);
}
