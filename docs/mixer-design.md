# Mixer Design — Hybrid Tank Drive

## Overview

ESP32-S3 nhận tín hiệu M-Bus từ MC7RE V2 receiver (Futaba SBUS-like, 100kbaud),
mix thành 2 kênh motor trái/phải, gửi qua Serial Simplified tới MDDS30 driver.

## Channel Mapping

| CH | Chức năng | Stick | Raw range |
|----|-----------|-------|-----------|
| 1  | AIL (Steer) | Phải ngang | 563–1356 (center ~960) |
| 2  | ELE (Throttle) | Phải dọc | 592–1390 (center ~985) |
| 3  | THR | Trái dọc | Không dùng cho drive |
| 4  | RUD | Trái ngang | Không dùng cho drive |
| 5  | SWA | Switch 3 vị trí | 200 / 1000 / 1800 |
| 6  | SWB | Switch 3 vị trí | 200 / 1000 / 1800 |
| 7  | Custom | — | Luôn 200 (không active) |

## Mixer Algorithm — Hybrid

### Không ga (throttle == 0): Tank Pivot
```
L =  steer
R = -steer
```
Xoay tại chỗ. Stick phải → bánh trái tiến, bánh phải lùi.

### Có ga (throttle != 0): Vòng cung
```
steer_ratio = |steer| / speed_max        (0.0 → 1.0)
inner       = throttle × (1 - steer_ratio)
outer       = throttle

steer > 0 (phải): L = outer,  R = inner
steer < 0 (trái): L = inner,  R = outer
```
Cả 2 bánh cùng chiều với throttle. Steer chỉ giảm tốc bánh bên trong.

### So sánh với Arcade Drive cũ

| Góc | Arcade (cũ) | Hybrid (mới) |
|-----|-------------|--------------|
| 0° ↑ tiến | L=28, R=28 | L=28, R=28 |
| 45° ↗ tiến-phải | L=55, R=**3** | L=29, R=**17** |
| 90° → xoay phải | L=26, R=-26 | L=26, R=-26 |
| 135° ↖ tiến-trái | L=**-3**, R=61 | L=**15**, R=29 |
| 180° ↓ lùi | L=-29, R=-29 | L=-29, R=-29 |

Arcade drive ở góc chéo: 1 bánh gần đứng yên hoặc đảo chiều.
Hybrid: cả 2 bánh cùng chiều, bánh trong chậm hơn.

## MC7RE V2 Failsafe

MC7RE V2 **không set** SBUS flag bit 2 (frame lost) / bit 3 (failsafe) chuẩn.
Thay vào đó dùng **bit 4** (0x10) trong byte 23 để báo mất sóng TX.

| Trạng thái | Byte 23 | Hành vi receiver |
|------------|---------|------------------|
| Có sóng | 0x00 | Gửi frame bình thường |
| Mất sóng | 0x10 | Gửi vài frame với flag, rồi ngừng hẳn |

Firmware detect cả 3 cơ chế:
1. `MBUS_FLAG_FAILSAFE` (bit 3) — SBUS chuẩn
2. `MBUS_FLAG_FRAME_LOST` (bit 2) — SBUS chuẩn
3. `MBUS_FLAG_SIGNAL_LOST` (bit 4) — MC7RE V2 specific
4. Frame timeout 100ms — receiver ngừng gửi

## Wiring

```
Battery (7–35V) ── MDDS30 [V+/GND]
BEC 5V          ── ESP32-S3 [VIN/GND]
ESP32-S3 [3.3V] ── MC7RE [+]
ESP32-S3 [GND]  ── MC7RE [−]
MC7RE [SIG]     ── ESP32-S3 [GPIO4]   (UART1 RX, inverted, 100kbaud 8E2)
ESP32-S3 [GPIO5]── MDDS30 [IN1]       (UART2 TX, 9600baud 8N1)
ESP32-S3 [GND]  ── MDDS30 [GND]
```

## MDDS30 DIP Switch

```
SW: 1=ON 2=ON 3=OFF 4=OFF 5=ON 6=ON 7=OFF 8=OFF
    ──── Serial mode ────  ── 9600 bps ──  ─ No bat mon ─
```

## Build Environments

| Env | Board | Flash | PSRAM | RGB LED pin |
|-----|-------|-------|-------|-------------|
| `esp32s3` | lolin_s3_mini | 4MB | 2MB QIO | GPIO38 (RGB_BUILTIN) |
| `esp32s3_n16r8` | lolin_s3 | 16MB | 8MB OPI | GPIO48 (`-DRGB_LED_PIN=48`) |

N16R8: PSRAM dùng `board_build.arduino.memory_type = qio_opi`. RGB LED là WS2812B trên
GPIO48 — **không phải** GPIO38 như board lolin_s3 chính hãng.

## Safety Notes (ESP32-S3 Super Mini)

Các vấn đề đã xử lý:

| # | Vấn đề | Fix |
|---|--------|-----|
| B1 | USB CDC `Serial.printf` block khi không có host | `if (Serial)` guard + rate-limit 100ms |
| B2 | `mdds.stop()` spam mỗi `loop()` khi timeout | Flag `motors_stopped` — chỉ gửi 1 lần |
| B3 | Buffer overrun tiềm ẩn trong MBUS reader | Đổi `== MBUS_FRAME_SIZE` → `>=` |
| B4 | Division by zero trong `mixer_map_channel` | Guard `if (span == 0) return 0` |
| B5 | Startup race: timeout trigger trước frame đầu | `last_frame_ms = millis()` trong setup() |

## Known Limitations

- Config calibration (MBUS_CENTER/MIN/MAX) chưa match stick thực tế
  - Config: 200–1800, center=992
  - Thực tế: ~560–1390, center=~970
  - Speed max chỉ đạt ~46% (29/63)
- CH7 luôn = 200, không active trên MC6C mini
- Hybrid mixer có điểm nhảy đột ngột tại ranh giới thr=0 ↔ thr=1
