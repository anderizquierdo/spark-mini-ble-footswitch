#include <unity.h>
#include "commandbuilder.h"


void test_simple_array() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendByte(0x01);
    commandBuilder.appendByte(0x20);
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x00, 0x01, 0x20,
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_simple_array_with_big_value() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendByte(0x01);
    commandBuilder.appendByte(0x82);
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x02, 0x01, 0x02,
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_multiple_pages_array_with_big_values() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendByte(0x01);
    commandBuilder.appendByte(0x82);
    commandBuilder.appendByte(0x03);
    commandBuilder.appendByte(0x04);
    commandBuilder.appendByte(0x05);
    commandBuilder.appendByte(0x06);
    commandBuilder.appendByte(0x07);
    commandBuilder.appendByte(0x08);
    commandBuilder.appendByte(0x89);
    commandBuilder.appendByte(0x10);
    commandBuilder.appendByte(0x11);
    commandBuilder.appendByte(0x12);
    commandBuilder.appendByte(0x13);
    commandBuilder.appendByte(0x14);
    commandBuilder.appendByte(0x15);
    commandBuilder.appendByte(0x16);
    commandBuilder.appendByte(0x17);
    commandBuilder.appendByte(0x18);
    commandBuilder.appendByte(0x19);
    commandBuilder.appendByte(0x20);
    commandBuilder.appendByte(0x21);
    commandBuilder.appendByte(0xA2);
    commandBuilder.appendByte(0x23);
    commandBuilder.appendByte(0xA4);
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x02,   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x02,   0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x00,   0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
        0x05,   0x22, 0x23, 0x24,
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_string() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendString("Overdrive");
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x01, 0x29, 'O', 'v', 'e', 'r', 'd', 'r', 
        0x00, 'i', 'v', 'e',
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_string_alt() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendStringAlt("Overdrive");
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x02, 0x09, 0x29, 'O', 'v', 'e', 'r', 'd', 
        0x00, 'r', 'i', 'v', 'e',
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_boolean_true() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendBoolean(true);
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x01, 0x43,
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void test_boolean_false() {
    SparkCommandBuilder commandBuilder(3, 0x05, 0x35);
    commandBuilder.appendBoolean(false);
    std::vector<uint8_t> command = commandBuilder.getBytes();

    uint8_t expectedCommand[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        3,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x05, 0x35,

        // Encoded data
        0x01, 0x42,
    
        // Footer
        0xF7
    };

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommand, command.data(), sizeof(expectedCommand));
}


void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_simple_array);
    RUN_TEST(test_simple_array_with_big_value);
    RUN_TEST(test_multiple_pages_array_with_big_values);
    RUN_TEST(test_string);
    RUN_TEST(test_string_alt);
    RUN_TEST(test_boolean_true);
    RUN_TEST(test_boolean_false);
    UNITY_END();
}

void loop() {

}
