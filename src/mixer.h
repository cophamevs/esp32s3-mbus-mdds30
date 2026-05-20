#pragma once
#include <stdint.h>

// Maps a raw MBUS channel value → signed speed in range [-speed_max, +speed_max].
// Returns 0 if raw is within dead_zone of center.
int8_t mixer_map_channel(uint16_t raw,
                          uint16_t center,
                          uint16_t dead_zone,
                          uint16_t raw_min,
                          uint16_t raw_max,
                          int8_t   speed_max);

// Differential drive mixing.
// throttle, steer: [-speed_max, +speed_max]
// left, right: result clamped to [-speed_max, +speed_max]
void mixer_compute(int8_t  throttle,
                   int8_t  steer,
                   int8_t  speed_max,
                   int8_t* left,
                   int8_t* right);
