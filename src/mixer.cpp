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
    if (throttle == 0) {
        // No throttle: spin in place (tank pivot)
        *left  = clamp8((int16_t)steer, -speed_max, speed_max);
        *right = clamp8((int16_t)-steer, -speed_max, speed_max);
    } else {
        // With throttle: both wheels same direction, steer reduces one side
        // steer_ratio = |steer| / speed_max  →  0.0 .. 1.0
        // inner wheel = throttle * (1 - steer_ratio)
        int16_t abs_str = steer < 0 ? -steer : steer;
        int16_t inner = (int16_t)throttle * (speed_max - abs_str) / speed_max;

        if (steer >= 0) {
            *left  = throttle;                                    // outer
            *right = clamp8(inner, -speed_max, speed_max);        // inner
        } else {
            *left  = clamp8(inner, -speed_max, speed_max);        // inner
            *right = throttle;                                    // outer
        }
    }
}
