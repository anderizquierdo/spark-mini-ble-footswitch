#ifndef COMMAND_BUILDER_H
#define COMMAND_BUILDER_H

#include <cstdint>
#include <vector>

class SparkCommandBuilder {
    public:
        SparkCommandBuilder(uint8_t sequenceNumber, uint8_t command, uint8_t subcommand);
        void appendByte(uint8_t value);
        void appendString(const char* value);
        void appendStringAlt(const char* value);
        void appendBoolean(bool value);
        std::vector<uint8_t> getBytes();
    private:
        // Constants
        static const uint8_t DATA_INITIAL_CAPACITY = 64;
        
        // Properties
        std::vector<uint8_t> data;
        uint8_t command;
        uint8_t subcommand;
        uint8_t sequenceNumber;

        // Methods
        std::vector<uint8_t> getEncodedData();
};

#endif
