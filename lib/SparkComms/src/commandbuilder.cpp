/*
    Special thanks to Paul Hamshere for his incredible contribution about the Spark's protocol.
        https://github.com/paulhamsh

    Check the page 3 - "Data Format" here for more information about the encoding:
        https://github.com/paulhamsh/Spark/blob/main/Spark%20Protocol%20Description%20v3.2.pdf
*/

#include <vector>
#include <string>
#include "commandbuilder.h"

SparkCommandBuilder::SparkCommandBuilder(uint8_t sequenceNumber, uint8_t command, uint8_t subcommand) {
    this->data.reserve(DATA_INITIAL_CAPACITY);
    this->command = command;
    this->subcommand = subcommand;
    this->sequenceNumber = sequenceNumber;
}

void SparkCommandBuilder::appendByte(uint8_t value) {
    this->data.push_back(value);
}

void SparkCommandBuilder::appendString(const char* value) {
    std::string valueStr(value);
    uint8_t strLength = static_cast<uint8_t>(valueStr.length());
    this->data.push_back(strLength + 0xA0);
    this->data.insert(this->data.end(), valueStr.begin(), valueStr.end());
}

void SparkCommandBuilder::appendStringAlt(const char* value) {
    std::string valueStr(value);
    uint8_t strLength = static_cast<uint8_t>(valueStr.length());
    this->data.push_back(strLength);
    this->data.push_back(strLength + 0xA0);
    this->data.insert(this->data.end(), valueStr.begin(), valueStr.end());
}

void SparkCommandBuilder::appendBoolean(bool value) {
    this->data.push_back(value ? 0xC3 : 0xC2);
}

std::vector<uint8_t> SparkCommandBuilder::getBytes() {
    const uint8_t header[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        this->sequenceNumber,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        this->command, this->subcommand
    };

    const uint8_t footer[] = {
        0xF7
    };

    std::vector<uint8_t> encodedData = getEncodedData();
    std::vector<uint8_t> command;
    command.reserve(sizeof(header) + encodedData.size() + sizeof(footer));

    command.insert(command.end(), std::begin(header), std::end(header));
    command.insert(command.end(), encodedData.begin(), encodedData.end());
    command.insert(command.end(), std::begin(footer), std::end(footer));

    return command;
}

std::vector<uint8_t> SparkCommandBuilder::getEncodedData() {
    size_t dataSize = this->data.size();
    uint numPages = (dataSize / 7) + (dataSize % 7 == 0 ? 0 : 1);

    std::vector<uint8_t> encodedData(dataSize + numPages, 0x00);

    uint currentPage = -1;
    for (int i = 0; i < dataSize; i++) {
        // Update page every 7 elements
        if (i % 7 == 0) {
            currentPage++;
        }

        // Calculate value index and encoded 8-bit byte index in the destination array
        uint encodedValueIndex = i + currentPage + 1;
        uint encoded8bitIndex = currentPage * 8;
        uint encoded8bitPositionInByte = 7 - (i % 7);

        uint8_t bitMask = 1 << 7; // 8th bit in the byte mask
        encodedData[encodedValueIndex] = this->data[i] & ~bitMask; // clears the most significant bit
        encodedData[encoded8bitIndex] |= (this->data[i] & bitMask) >> encoded8bitPositionInByte; // sets the right bit in the 8th-bits byte of the sequence
    }
    return encodedData;
}
