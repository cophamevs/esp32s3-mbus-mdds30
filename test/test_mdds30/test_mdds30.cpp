#include <unity.h>
#include "../../src/mdds30_serial.h"

// ── LEFT motor encoding ────────────────────────────────────
// Bit7=0 (LEFT), Bit6=0 (CW/forward), Bits5:0=speed

void test_encode_left_stop() {
    TEST_ASSERT_EQUAL_HEX8(0x00, mdds30_encode_left(0));
}

void test_encode_left_forward_full() {
    // speed=63: 0b00_111111 = 0x3F
    TEST_ASSERT_EQUAL_HEX8(0x3F, mdds30_encode_left(63));
}

void test_encode_left_forward_half() {
    // speed=31: 0b00_011111 = 0x1F
    TEST_ASSERT_EQUAL_HEX8(0x1F, mdds30_encode_left(31));
}

void test_encode_left_reverse_full() {
    // speed=-63: Bit6=1 (CCW), 0b01_111111 = 0x7F
    TEST_ASSERT_EQUAL_HEX8(0x7F, mdds30_encode_left(-63));
}

void test_encode_left_reverse_half() {
    // speed=-31: 0b01_011111 = 0x5F
    TEST_ASSERT_EQUAL_HEX8(0x5F, mdds30_encode_left(-31));
}

// ── RIGHT motor encoding ───────────────────────────────────
// Bit7=1 (RIGHT), Bit6=0 (CW/forward), Bits5:0=speed

void test_encode_right_stop() {
    TEST_ASSERT_EQUAL_HEX8(0x80, mdds30_encode_right(0));
}

void test_encode_right_forward_full() {
    // speed=63: 0b10_111111 = 0xBF
    TEST_ASSERT_EQUAL_HEX8(0xBF, mdds30_encode_right(63));
}

void test_encode_right_forward_half() {
    // speed=31: 0b10_011111 = 0x9F
    TEST_ASSERT_EQUAL_HEX8(0x9F, mdds30_encode_right(31));
}

void test_encode_right_reverse_full() {
    // speed=-63: 0b11_111111 = 0xFF
    TEST_ASSERT_EQUAL_HEX8(0xFF, mdds30_encode_right(-63));
}

void test_encode_right_reverse_half() {
    // speed=-31: 0b11_011111 = 0xDF
    TEST_ASSERT_EQUAL_HEX8(0xDF, mdds30_encode_right(-31));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_encode_left_stop);
    RUN_TEST(test_encode_left_forward_full);
    RUN_TEST(test_encode_left_forward_half);
    RUN_TEST(test_encode_left_reverse_full);
    RUN_TEST(test_encode_left_reverse_half);
    RUN_TEST(test_encode_right_stop);
    RUN_TEST(test_encode_right_forward_full);
    RUN_TEST(test_encode_right_forward_half);
    RUN_TEST(test_encode_right_reverse_full);
    RUN_TEST(test_encode_right_reverse_half);
    return UNITY_END();
}
