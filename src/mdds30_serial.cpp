#include "mdds30_serial.h"

// ── Pure encoding ─────────────────────────────────────────

uint8_t mdds30_encode_left(int8_t speed) {
    if (speed >= 0)
        return 0x00 | (uint8_t)(speed & 0x3F);          // CW
    else
        return 0x40 | (uint8_t)((-speed) & 0x3F);       // CCW
}

uint8_t mdds30_encode_right(int8_t speed) {
    if (speed >= 0)
        return 0x80 | (uint8_t)(speed & 0x3F);          // CW
    else
        return 0xC0 | (uint8_t)((-speed) & 0x3F);       // CCW
}

// ── Hardware sender ───────────────────────────────────────
#ifdef ARDUINO

void Mdds30Serial::begin(HardwareSerial& uart, int tx_pin, unsigned long baud) {
    _uart = &uart;
    _uart->begin(baud, SERIAL_8N1, -1, tx_pin);
}

void Mdds30Serial::send(int8_t left, int8_t right) {
    uint8_t buf[2] = { mdds30_encode_left(left), mdds30_encode_right(right) };
    _uart->write(buf, 2);
}

void Mdds30Serial::stop() {
    send(0, 0);   // → 0x00 (LEFT stop), 0x80 (RIGHT stop)
}

#endif
