#include "sparkcomms.h"
#include <unity.h>


void test_simple_array() {
    uint8_t payload[] = {0x01, 0x20};
    uint8_t expectedPayload[] = {0x00, 0x01, 0x20};

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_simple_array_with_big_value() {
    uint8_t payload[] = {0x01, 0x82};
    uint8_t expectedPayload[] = {0x02, 0x01, 0x02};

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_multiple_pages_array_with_big_values() {
    uint8_t payload[] = {
        0x01, 0x82, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x89, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
        0xA2, 0x23, 0xA4
    };
    uint8_t expectedPayload[] = {
        0x02,   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x02,   0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x00,   0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
        0x05,   0x22, 0x23, 0x24
    };

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_overdrive_toggle() {
    uint8_t payload[] = {
        0x09, 0xA9, 'O', 'v', 'e', 'r', 'd', 
        'r', 'i', 'v', 'e', 0xc3
    };
    uint8_t expectedPayload[] = {
        0x02, 0x09, 0x29, 'O', 'v', 'e', 'r', 'd', 
        0x10, 'r', 'i', 'v', 'e', 0x43
    };

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_fuzz_toggle() {
    uint8_t payload[] = {
        0x04, 0xA4, 'F', 'u', 'z', 'z', 0xc3
    };
    uint8_t expectedPayload[] = {
        0x42, 0x04, 0x24, 'F', 'u', 'z', 'z', 0x43
    };

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_tubescreamer_toggle() {
    uint8_t payload[] = {
        0x0D, 0xAD, 'D', 'i', 's', 't', 'o', 
        'r', 't', 'i', 'o', 'n', 'T', 'S', 
        '9', 0xc3
    };
    uint8_t expectedPayload[] = {
        0x02, 0x0D, 0x2D, 'D', 'i', 's', 't', 'o', 
        0x00, 'r', 't', 'i', 'o', 'n', 'T', 'S', 
        0x02, '9', 0x43
    };

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void test_booster_toggle() {
    uint8_t payload[] = {
        0x07, 0xA7, 'B', 'o', 'o', 's', 't', 
        'e', 'r', 0xc3
    };
    uint8_t expectedPayload[] = {
        0x02, 0x07, 0x27, 'B', 'o', 'o', 's', 't', 
        0x04, 'e', 'r', 0x43
    };

    uint lenght = sizeof(payload) / sizeof(payload[0]);
    uint encodedLenght;

    uint8_t* encodedPayload = encodePayload(payload, lenght, &encodedLenght);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPayload, encodedPayload, encodedLenght);
}


void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_simple_array);
    RUN_TEST(test_simple_array_with_big_value);
    RUN_TEST(test_multiple_pages_array_with_big_values);
    RUN_TEST(test_overdrive_toggle);
    RUN_TEST(test_fuzz_toggle);
    RUN_TEST(test_tubescreamer_toggle);
    RUN_TEST(test_booster_toggle);
    UNITY_END();
}

void loop() {

}
