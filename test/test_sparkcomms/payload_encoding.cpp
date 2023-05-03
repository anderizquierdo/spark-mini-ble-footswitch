#include "sparkcomms.h"
#include <unity.h>


void test_dummy() {
    const uint8_t payload[] = {0x01, 0x20};
    const uint8_t* encodedPayload = encodePayload(payload);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(payload, encodedPayload, 2);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    UNITY_END();
}

void loop() {

}
