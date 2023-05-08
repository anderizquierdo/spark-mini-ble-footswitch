#include "sparkcomms.h"
#include "commandbuilder.h"


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
    SparkCommandBuilder commandBuilder(++sequenceNumber, 0x01, 0x38);
    commandBuilder.appendByte(0x00);
    commandBuilder.appendByte(presetNum);
    pCharSender->writeValue(commandBuilder.getBytes());
}

void SparkMiniComms::setDrive(const char* pedal, bool active) {
    SparkCommandBuilder commandBuilder(++sequenceNumber, 0x01, 0x15);
    commandBuilder.appendStringAlt(pedal);
    commandBuilder.appendBoolean(active);
    pCharSender->writeValue(commandBuilder.getBytes());
}

void SparkMiniComms::getCurrentPresetInfo() {
    SparkCommandBuilder commandBuilder(++sequenceNumber, 0x02, 0x01);
    // Current live preset is 0x01 0x00
    commandBuilder.appendByte(0x01);
    commandBuilder.appendByte(0x00);

    // 30 bytes of 0x00
    for(int i = 0; i < 30; i++) {
        commandBuilder.appendByte(0x00);
    }

    pCharSender->writeValue(commandBuilder.getBytes());
}
