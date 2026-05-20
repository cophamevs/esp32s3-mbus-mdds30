#pragma once
#include <stdint.h>

#ifdef ARDUINO
#include <HardwareSerial.h>

#define MBUS_FRAME_SIZE       25
#define MBUS_START_BYTE       0x0F
#define MBUS_END_BYTE         0x00
#define MBUS_NUM_CHANNELS     16
#define MBUS_FLAG_FRAME_LOST  (1 << 2)
#define MBUS_FLAG_FAILSAFE    (1 << 3)
#define MBUS_FLAG_SIGNAL_LOST (1 << 4)  // MC7RE V2: bit 4 = signal lost
#define MBUS_INTER_FRAME_GAP  3    // ms: minimum gap between frames

class MbusReader {
public:
    // rx_pin: GPIO receiving MBUS SIG (inverted serial, 100kbaud, 8E2)
    void begin(HardwareSerial& uart, int rx_pin);

    // Call in loop(). Returns true when a valid frame is received.
    // channels: 16-element array, values 0–2047 (MBUS center ~992)
    // failsafe: true if TX lost or frame lost
    bool read(uint16_t channels[MBUS_NUM_CHANNELS], bool* failsafe, uint8_t* flags_out = nullptr);

private:
    HardwareSerial* _uart         = nullptr;
    uint8_t         _buf[MBUS_FRAME_SIZE];
    uint8_t         _count        = 0;
    unsigned long   _last_byte_ms = 0;

    void decode(uint16_t channels[MBUS_NUM_CHANNELS], bool* failsafe, uint8_t* flags_out);
};
#endif
