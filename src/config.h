#pragma once
#include <stdint.h>

// ── GPIO Pins ──────────────────────────────────────────────
#define MBUS_RX_PIN       4     // MC7RE M.BUS SIG → GPIO4 (UART1 RX, inverted)
#define MDDS30_TX_PIN     5     // MDDS30 IN1 ← GPIO5 (UART2 TX)

// ── Baud Rates ─────────────────────────────────────────────
#define MBUS_BAUD         100000
#define MDDS30_BAUD       9600

// ── Channel Mapping (1-indexed, matches MC7RE CH label) ───
#define CH_STEER          1     // CH1 = AIL = right stick horizontal
#define CH_THROTTLE       2     // CH2 = ELE = right stick vertical

// ── MBUS Channel Calibration ───────────────────────────────
#define MBUS_CENTER       992   // raw value at stick center
#define MBUS_DEAD_ZONE    50    // ±50 quanh center → speed 0
#define MBUS_MIN          200   // raw value at full negative
#define MBUS_MAX          1800  // raw value at full positive

// ── MDDS30 Speed Range ─────────────────────────────────────
#define SPEED_MAX         63    // Serial Simplified max speed (6-bit, 0–63)

// ── Failsafe Timeout ───────────────────────────────────────
#define MBUS_TIMEOUT_MS   100   // stop motors nếu không có frame hợp lệ >100ms
