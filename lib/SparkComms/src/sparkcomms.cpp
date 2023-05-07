#include "sparkcomms.h"

// Check the page 3 - "Data Format" here for more information about the encoding:
// https://github.com/paulhamsh/Spark/blob/main/Spark%20Protocol%20Description%20v3.2.pdf
// Special thanks to Paul Hamshere for his incredible contribution about the Spark's protocol.
// https://github.com/paulhamsh
uint8_t* encodePayload(const uint8_t* pData, const size_t length, size_t* pReturnedLength) {
    uint numPages = (length / 7) + (length % 7 == 0 ? 0 : 1);
    *pReturnedLength = length + numPages;
    uint8_t* encodedPayload = new uint8_t[*pReturnedLength];

    for (int i=0; i<*pReturnedLength; i++) {
        encodedPayload[i] = 0; // fill with zeros
    }

    uint currentPage = -1;
    for (int i=0; i<length; i++) {
        // Update page every 7 elements
        if (i % 7 == 0) {
            currentPage++;
        }

        // Calculate value index and encoded 8-bit byte index in the destination array
        uint encodedValueIndex = i + currentPage + 1;
        uint encoded8bitIndex = currentPage * 8;
        uint encoded8bitPosition = 7 - (i % 7);

        uint8_t bitMask = 1 << 7; // 8th bit in the byte mask
        encodedPayload[encodedValueIndex] = pData[i] & ~bitMask; // clears the most significant bit
        encodedPayload[encoded8bitIndex] |= (pData[i] & bitMask) >> encoded8bitPosition; // sets the right bit in the 8th-bits byte of the sequence
    }
    return encodedPayload;
}

void notifyCallBack(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  int i;
  Serial.print("From Spark: ");
  for (i = 0; i < length; i++) {
    if (pData[i] < 0x10) {
        Serial.print(0);
    }
    Serial.print(pData[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}

bool SparkMiniComms::connect() {
    if (!NimBLEDevice::getInitialized()) {
        NimBLEDevice::init("SparkMiniFootSwitch");
    }

    // BLE scan
    NimBLEScan *pScan = NimBLEDevice::getScan();
    Serial.println("Starting BLE devices scan...");
    //TO-DO: make this async with a callback function
    NimBLEScanResults results = pScan->start(5);
    Serial.println("BLE devices scan completed.");

    // Service discovery
    NimBLEUUID targetService(serviceUUID);
    NimBLEAdvertisedDevice device;
    bool deviceFound = false;

    for(int i = 0; i < results.getCount() && !deviceFound; i++) {
        device = results.getDevice(i);
    
        if (device.isAdvertisingService(targetService)) {
            deviceFound = true;
        }
    }

    if (!deviceFound) {
        Serial.println("Spark not found!");
        return false;
    }

    Serial.println("Spark found!");
    
    // Connection to device
    pClient = NimBLEDevice::createClient();
    Serial.println("Connecting...");
    if (!pClient->connect(&device)) {
        Serial.println("Error connecting to the Spark!");
        disconnect();
        return false;
    }

    Serial.println("Connected!");
    Serial.println("Starting characteristics discovery...");

    // Characteristics discovery
    NimBLERemoteService *pService = pClient->getService(targetService);
    pCharSender = pService->getCharacteristic(charSenderUUID);
    if (pCharSender == NULL) {
        Serial.println("Error setting up sender channel!");
        disconnect();
        return false;
    }
    pCharReceiver = pService->getCharacteristic(charReceiverUUID);
    if (pCharReceiver == NULL) {
        Serial.println("Error setting up receiver channel!");
        disconnect();
        return false;
    }

    if (pCharReceiver->canNotify()) {
        if (!pCharReceiver->subscribe(true, notifyCallBack, true)) {
            Serial.println("Error setting up receiver channel!");
            disconnect();
            return false;
        }
    } 

    Serial.println("Successfully connected. Congrats!");
    return true;
}

bool SparkMiniComms::isConnected() {
    return ((pClient != NULL) && (pClient->isConnected()));
}

void SparkMiniComms::disconnect() {
    NimBLEDevice::deleteClient(pClient);
    pClient = NULL;
}

void SparkMiniComms::setPreset(u_int8_t presetNum) {
    u_int8_t data[] = {
        0x00, presetNum
    };

    sendCommand(0x01, 0x38, data, sizeof(data));
}

void SparkMiniComms::setDrive(const char* pedal, bool active) {
    size_t pedalLen = strlen(pedal);
    size_t dataSize = pedalLen + 3;
    
    uint8_t* data = new uint8_t[dataSize];
    data[0] = pedalLen;
    data[1] = pedalLen + 0xA0;
    memcpy(&data[2], pedal, pedalLen);
    data[dataSize - 1] = active ? 0xc3 : 0xc2;
    
    sendCommand(0x01, 0x15, data, dataSize);
    delete[] data;
}

void SparkMiniComms::getCurrentPresetInfo() {
    uint8_t data[] = {
        // Current live preset
        0x01, 0x00,

        // 30 bytes of 0x00
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    sendCommand(0x02, 0x01, data, sizeof(data));
}

void SparkMiniComms::sendCommand(uint8_t command, uint8_t subcommand, const uint8_t *data, size_t dataSize) {
    uint8_t cmdHeader[] = {
        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        ++sequenceNumber,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        command, subcommand
    };
    uint headerSize = sizeof(cmdHeader);

    uint8_t cmdFooter[] = {
        0xF7
    };
    uint footerSize = sizeof(cmdFooter);

    uint encodedDataSize;
    uint8_t* encodedData = encodePayload(data, dataSize, &encodedDataSize);

    uint messageLenght = headerSize + encodedDataSize + footerSize;
    uint8_t* message = new uint8_t[messageLenght];
    uint8_t* p = message;

    memcpy(p, cmdHeader, headerSize);
    p += headerSize;
    memcpy(p, encodedData, encodedDataSize);
    p += encodedDataSize;
    memcpy(p, cmdFooter, footerSize);
  
    pCharSender->writeValue(message, messageLenght);
    delete[] encodedData;
    delete[] message;
}
