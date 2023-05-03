#include "sparkcomms.h"


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

void SparkMiniComms::setPreset(int presetNum) {
    byte presetCmd[] = {
        0x01, 0xFE, 0x00, 0x00,
        0x53, 0xFE, 0x1A, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xF0, 0x01, 0x24, 0x00,
        0x01, 0x38, 0x00, 0x00,
        0x00, 0xF7
    };
    int presetCmdSize = sizeof(presetCmd) / sizeof(presetCmd[0]);
    presetCmd[presetCmdSize - 2] = presetNum;
    pCharSender->writeValue(presetCmd, presetCmdSize);
}

void SparkMiniComms::setDrive(bool active) {
    byte status = active ? 0x43 : 0x42;
    byte sparkCmd[] = {
        // HEADER
        0x01, 0xFE, 0x00, 0x00,

        // Direction of the message, where 0x53fe is "to Spark"
        0x53, 0xFE,
        
        // Size of this block (including this header) -> will be processed later in the code
        0x00, 
        
        // Zeros (9 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        // CHUNK HEADER
        0xF0, 0x01, 
        
        // Sequence number
        0x24,
        
        // Checksum (8 bit Xor)
        0x00,

        // Command
        0x01, 0x15,
        
        // Data
        0x02, 0x09, 0x29, 'O', 'v', 'e', 'r', 'd', 
        0x10, 'r', 'i', 'v', 'e', status, 

        // CHUNK FOOTER
        0xF7
    };
    int sparkCmdSize = sizeof(sparkCmd) / sizeof(sparkCmd[0]);
    sparkCmd[6] = (unsigned char) sparkCmdSize;
    pCharSender->writeValue(sparkCmd, sparkCmdSize);
}
