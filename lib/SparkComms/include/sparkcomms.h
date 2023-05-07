#ifndef SPARK_COMMS_H
#define SPARK_COMMS_H

#include <NimBLEDevice.h>

uint8_t* encodePayload(const uint8_t* pData, const size_t length, size_t* pReturnedLength);

class SparkMiniComms {
  private:
    const char *serviceUUID = "ffc0";
    const char *charSenderUUID = "ffc1";
    const char *charReceiverUUID = "ffc2";
    NimBLEClient *pClient;
    NimBLERemoteCharacteristic *pCharSender;
    NimBLERemoteCharacteristic *pCharReceiver;
    void sendCommand(uint8_t command, uint8_t subcommand, const uint8_t *data);

  public:
    bool connect();
    bool isConnected();
    void disconnect();
    void setPreset(u_int8_t presetNum);
    void setDrive(bool active);
    void getCurrentPresetInfo();
};

#endif