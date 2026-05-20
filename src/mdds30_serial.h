#pragma once
#include <stdint.h>

// ── Pure encoding — no Arduino dependency, testable natively ──
//
// speed: -63 (full reverse) .. 0 (stop) .. +63 (full forward)
//
// MDDS30 Serial Simplified byte format:
//   Bit 7  = motor select  (0=LEFT, 1=RIGHT)
//   Bit 6  = direction     (0=CW/forward, 1=CCW/reverse)
//   Bit5:0 = speed         (0=stop, 63=full)
uint8_t mdds30_encode_left(int8_t speed);
uint8_t mdds30_encode_right(int8_t speed);

// ── Hardware sender — requires Arduino ──────────────────────
#ifdef ARDUINO
#include <HardwareSerial.h>

class Mdds30Serial {
public:
    void begin(HardwareSerial& uart, int tx_pin, unsigned long baud);
    void send(int8_t left, int8_t right);
    void stop();

private:
    HardwareSerial* _uart = nullptr;
};
#endif
