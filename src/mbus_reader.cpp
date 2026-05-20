#include "mbus_reader.h"

#ifdef ARDUINO

void MbusReader::begin(HardwareSerial& uart, int rx_pin) {
    _uart = &uart;
    // inverted=true: MBUS idle=LOW (opposite of normal UART)
    _uart->begin(100000, SERIAL_8E2, rx_pin, -1, true);
}

bool MbusReader::read(uint16_t channels[MBUS_NUM_CHANNELS], bool* failsafe, uint8_t* flags_out) {
    unsigned long now = millis();

    // Reset buffer on inter-frame gap (>3ms without a byte)
    if (_count > 0 && (now - _last_byte_ms) > MBUS_INTER_FRAME_GAP) {
        _count = 0;
    }

    while (_uart->available()) {
        uint8_t b = _uart->read();
        _last_byte_ms = millis();

        // Skip bytes until start byte 0x0F is found
        if (_count == 0 && b != MBUS_START_BYTE) continue;

        _buf[_count++] = b;

        if (_count == MBUS_FRAME_SIZE) {
            _count = 0;
            if (_buf[0] == MBUS_START_BYTE && _buf[24] == MBUS_END_BYTE) {
                decode(channels, failsafe, flags_out);
                return true;
            }
            // Invalid frame: wait for new start byte
        }
    }
    return false;
}

void MbusReader::decode(uint16_t channels[MBUS_NUM_CHANNELS], bool* failsafe, uint8_t* flags_out) {
    uint8_t* b = _buf;

    // Unpack 16 channels × 11-bit from 22 data bytes (bytes 1–22)
    channels[0]  = ((uint16_t)b[1]        | (uint16_t)b[2]  << 8)                           & 0x07FF;
    channels[1]  = ((uint16_t)b[2]  >> 3  | (uint16_t)b[3]  << 5)                           & 0x07FF;
    channels[2]  = ((uint16_t)b[3]  >> 6  | (uint16_t)b[4]  << 2  | (uint16_t)b[5]  << 10) & 0x07FF;
    channels[3]  = ((uint16_t)b[5]  >> 1  | (uint16_t)b[6]  << 7)                           & 0x07FF;
    channels[4]  = ((uint16_t)b[6]  >> 4  | (uint16_t)b[7]  << 4)                           & 0x07FF;
    channels[5]  = ((uint16_t)b[7]  >> 7  | (uint16_t)b[8]  << 1  | (uint16_t)b[9]  << 9)  & 0x07FF;
    channels[6]  = ((uint16_t)b[9]  >> 2  | (uint16_t)b[10] << 6)                           & 0x07FF;
    channels[7]  = ((uint16_t)b[10] >> 5  | (uint16_t)b[11] << 3)                           & 0x07FF;
    channels[8]  = ((uint16_t)b[12]       | (uint16_t)b[13] << 8)                           & 0x07FF;
    channels[9]  = ((uint16_t)b[13] >> 3  | (uint16_t)b[14] << 5)                           & 0x07FF;
    channels[10] = ((uint16_t)b[14] >> 6  | (uint16_t)b[15] << 2  | (uint16_t)b[16] << 10) & 0x07FF;
    channels[11] = ((uint16_t)b[16] >> 1  | (uint16_t)b[17] << 7)                           & 0x07FF;
    channels[12] = ((uint16_t)b[17] >> 4  | (uint16_t)b[18] << 4)                           & 0x07FF;
    channels[13] = ((uint16_t)b[18] >> 7  | (uint16_t)b[19] << 1  | (uint16_t)b[20] << 9)  & 0x07FF;
    channels[14] = ((uint16_t)b[20] >> 2  | (uint16_t)b[21] << 6)                           & 0x07FF;
    channels[15] = ((uint16_t)b[21] >> 5  | (uint16_t)b[22] << 3)                           & 0x07FF;

    uint8_t flags = b[23];
    if (flags_out) *flags_out = flags;
    *failsafe = (flags & MBUS_FLAG_FAILSAFE) || (flags & MBUS_FLAG_FRAME_LOST) || (flags & MBUS_FLAG_SIGNAL_LOST);
}

#endif
