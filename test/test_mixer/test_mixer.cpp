#include <unity.h>
#include "../../src/mixer.h"

// ── mixer_map_channel ──────────────────────────────────────

void test_center_returns_zero() {
    TEST_ASSERT_EQUAL(0, mixer_map_channel(992, 992, 50, 200, 1800, 63));
}

void test_dead_zone_upper_edge_returns_zero() {
    // 992 + 50 = 1042 → still inside dead zone
    TEST_ASSERT_EQUAL(0, mixer_map_channel(1042, 992, 50, 200, 1800, 63));
}

void test_dead_zone_lower_edge_returns_zero() {
    // 992 - 50 = 942 → still inside dead zone
    TEST_ASSERT_EQUAL(0, mixer_map_channel(942, 992, 50, 200, 1800, 63));
}

void test_max_raw_returns_speed_max() {
    TEST_ASSERT_EQUAL(63, mixer_map_channel(1800, 992, 50, 200, 1800, 63));
}

void test_min_raw_returns_neg_speed_max() {
    TEST_ASSERT_EQUAL(-63, mixer_map_channel(200, 992, 50, 200, 1800, 63));
}

void test_above_dead_zone_returns_positive() {
    int8_t v = mixer_map_channel(1100, 992, 50, 200, 1800, 63);
    TEST_ASSERT_GREATER_THAN(0, v);
    TEST_ASSERT_LESS_THAN(63, v);
}

void test_below_dead_zone_returns_negative() {
    int8_t v = mixer_map_channel(880, 992, 50, 200, 1800, 63);
    TEST_ASSERT_LESS_THAN(0, v);
    TEST_ASSERT_GREATER_THAN(-63, v);
}

// ── mixer_compute ──────────────────────────────────────────

void test_mix_forward_straight() {
    int8_t l, r;
    mixer_compute(63, 0, 63, &l, &r);
    TEST_ASSERT_EQUAL(63, l);
    TEST_ASSERT_EQUAL(63, r);
}

void test_mix_reverse_straight() {
    int8_t l, r;
    mixer_compute(-63, 0, 63, &l, &r);
    TEST_ASSERT_EQUAL(-63, l);
    TEST_ASSERT_EQUAL(-63, r);
}

void test_mix_steer_right_from_stop() {
    int8_t l, r;
    mixer_compute(0, 32, 63, &l, &r);
    TEST_ASSERT_EQUAL(32, l);
    TEST_ASSERT_EQUAL(-32, r);
}

void test_mix_steer_left_from_stop() {
    int8_t l, r;
    mixer_compute(0, -32, 63, &l, &r);
    TEST_ASSERT_EQUAL(-32, l);
    TEST_ASSERT_EQUAL(32, r);
}

void test_mix_forward_steer_right_clamp() {
    // throttle=63 + steer=63 → left=126 → clamp to 63
    int8_t l, r;
    mixer_compute(63, 63, 63, &l, &r);
    TEST_ASSERT_EQUAL(63, l);
    TEST_ASSERT_EQUAL(0, r);   // 63 - 63 = 0
}

void test_mix_stop_returns_zero() {
    int8_t l, r;
    mixer_compute(0, 0, 63, &l, &r);
    TEST_ASSERT_EQUAL(0, l);
    TEST_ASSERT_EQUAL(0, r);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_center_returns_zero);
    RUN_TEST(test_dead_zone_upper_edge_returns_zero);
    RUN_TEST(test_dead_zone_lower_edge_returns_zero);
    RUN_TEST(test_max_raw_returns_speed_max);
    RUN_TEST(test_min_raw_returns_neg_speed_max);
    RUN_TEST(test_above_dead_zone_returns_positive);
    RUN_TEST(test_below_dead_zone_returns_negative);
    RUN_TEST(test_mix_forward_straight);
    RUN_TEST(test_mix_reverse_straight);
    RUN_TEST(test_mix_steer_right_from_stop);
    RUN_TEST(test_mix_steer_left_from_stop);
    RUN_TEST(test_mix_forward_steer_right_clamp);
    RUN_TEST(test_mix_stop_returns_zero);
    return UNITY_END();
}
