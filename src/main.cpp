#include <Arduino.h>
#include "config.h"
#include "mbus_reader.h"
#include "mdds30_serial.h"
#include "mixer.h"

static MbusReader    mbus;
static Mdds30Serial  mdds;

static uint16_t      channels[MBUS_NUM_CHANNELS];
static bool          failsafe       = true;
static uint8_t       flags          = 0;
static unsigned long last_frame_ms  = 0;

void setup() {
    Serial.begin(115200);      // USB CDC — debug output
    delay(500);                // wait for USB CDC ready

    mbus.begin(Serial1, MBUS_RX_PIN);
    mdds.begin(Serial2, MDDS30_TX_PIN, MDDS30_BAUD);

    mdds.stop();               // ensure motors start stopped
    Serial.println("[MBUS] Bridge started. Waiting for signal...");
}

void loop() {
    if (mbus.read(channels, &failsafe, &flags)) {
        last_frame_ms = millis();

        if (failsafe) {
            mdds.stop();
            Serial.println("[MBUS] FAILSAFE — motors stopped");
            return;
        }

        // CH_STEER=1 → channels[0], CH_THROTTLE=2 → channels[1]
        int8_t throttle = mixer_map_channel(
            channels[CH_THROTTLE - 1],
            MBUS_CENTER, MBUS_DEAD_ZONE, MBUS_MIN, MBUS_MAX, SPEED_MAX);

        int8_t steer = mixer_map_channel(
            channels[CH_STEER - 1],
            MBUS_CENTER, MBUS_DEAD_ZONE, MBUS_MIN, MBUS_MAX, SPEED_MAX);

        int8_t left, right;
        mixer_compute(throttle, steer, SPEED_MAX, &left, &right);
        mdds.send(left, right);

        Serial.printf("[CH] 1=%4u 2=%4u 3=%4u 4=%4u 5=%4u 6=%4u 7=%4u | thr=%4d str=%4d | L=%4d R=%4d | 0x%02X\n",
            channels[0], channels[1], channels[2], channels[3],
            channels[4], channels[5], channels[6],
            throttle, steer, left, right, flags);
    }

    // Timeout failsafe: no valid frame for MBUS_TIMEOUT_MS
    if ((millis() - last_frame_ms) > MBUS_TIMEOUT_MS) {
        mdds.stop();
    }
}
