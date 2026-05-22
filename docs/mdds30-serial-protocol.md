# MDDS30 Serial Protocol

## Tổng quan

MDDS30 (SmartDriveDuo-30) hỗ trợ 4 input mode. Firmware này dùng **Serial Simplified**.

| Mode | SW1 | SW2 | SW3 | Ghi chú |
|------|-----|-----|-----|---------|
| RC | 0 | 0 | — | Radio control |
| Analog/PWM | 0/1 | 1/0 | — | Analog hoặc PWM |
| **Serial Simplified** | **1** | **1** | **0** | ← **Đang dùng** |
| Serial Packetized | 1 | 1 | 1 | Multi-drop, 4-byte packet |

---

## Serial Simplified — Cấu trúc packet

**1 byte duy nhất** cho mỗi lệnh motor.

```
 Bit:  7        6        5   4   3   2   1   0
       CHANNEL  DIR      ←──────── SPEED (6-bit) ──────────→
```

| Bit | Tên | `0` | `1` |
|-----|-----|-----|-----|
| 7 (MSB) | Channel | Motor LEFT | Motor RIGHT |
| 6 | Direction | CW (forward) | CCW (reverse) |
| 5..0 | Speed | 0 = stop | 63 = full speed |

### Encoding (từ speed signed -63..+63)

| Trường hợp | Mask hex | Mask nhị phân |
|-----------|---------|--------------|
| LEFT + forward (`speed ≥ 0`) | `0x00 \| speed` | `00xxxxxx` |
| LEFT + reverse (`speed < 0`) | `0x40 \| \|speed\|` | `01xxxxxx` |
| RIGHT + forward (`speed ≥ 0`) | `0x80 \| speed` | `10xxxxxx` |
| RIGHT + reverse (`speed < 0`) | `0xC0 \| \|speed\|` | `11xxxxxx` |

### Ví dụ giá trị byte

| Decimal | Binary | Ý nghĩa |
|---------|--------|---------|
| `0`   | `0000 0000` | LEFT — dừng |
| `63`  | `0011 1111` | LEFT — full speed forward |
| `64`  | `0100 0000` | LEFT — dừng (reverse, speed=0) |
| `127` | `0111 1111` | LEFT — full speed reverse |
| `128` | `1000 0000` | RIGHT — dừng |
| `191` | `1011 1111` | RIGHT — full speed forward |
| `255` | `1111 1111` | RIGHT — full speed reverse |

---

## Cấu hình phần cứng

### DIP Switch (MDDS30)

```
SW: 1=ON  2=ON  3=OFF  4=OFF  5=ON  6=ON  7=OFF  8=OFF
    ──Serial──   ──────── 9600 bps ────────  ──No bat──
```

- SW1=1, SW2=1, SW3=0 → Serial Simplified mode
- SW4=0, SW5=1, SW6=1 → Baudrate 9600 bps (011)

### Wiring

```
ESP32-S3 GPIO5 (UART2 TX) ──→ MDDS30 IN1
ESP32-S3 GND              ──→ MDDS30 GND
```

Chỉ dùng **1 dây TX** — không cần RX từ driver.

---

## Implementation

### Encoding (`mdds30_serial.cpp`)

```c
uint8_t mdds30_encode_left(int8_t speed) {
    if (speed >= 0)
        return 0x00 | (uint8_t)(speed & 0x3F);   // CW
    else
        return 0x40 | (uint8_t)((-speed) & 0x3F); // CCW
}

uint8_t mdds30_encode_right(int8_t speed) {
    if (speed >= 0)
        return 0x80 | (uint8_t)(speed & 0x3F);   // CW
    else
        return 0xC0 | (uint8_t)((-speed) & 0x3F); // CCW
}
```

### Gửi lệnh (`Mdds30Serial::send`)

```c
void Mdds30Serial::send(int8_t left, int8_t right) {
    uint8_t buf[2] = { mdds30_encode_left(left), mdds30_encode_right(right) };
    _uart->write(buf, 2);   // 2 byte liên tiếp: LEFT rồi RIGHT
}
```

Mỗi chu kỳ loop gửi **2 byte**: byte LEFT motor + byte RIGHT motor.

---

## So sánh với Serial Packetized (không dùng)

| Tiêu chí | **Simplified (đang dùng)** | Packetized |
|----------|--------------------------|-----------|
| Bytes/lệnh | **1** | 4 |
| Baudrate | Fixed DIP switch | Auto-detect (gửi `0x80` lúc boot) |
| Multi-drop | Không | Tối đa 8 driver |
| Stop value | speed bits = 0 | Byte command = `127` |
| Độ phân giải | 64 mức/chiều | **128 mức/chiều** |
| Checksum | Không | Có |

Simplified đủ dùng cho hệ 1 driver, code đơn giản hơn, không cần init sequence.
