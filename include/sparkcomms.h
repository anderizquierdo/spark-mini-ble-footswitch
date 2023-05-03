#ifndef SPARK_COMMS_H
#define SPARK_COMMS_H

#include <NimBLEDevice.h>

class SparkMiniComms {
  private:
    const char *serviceUUID = "ffc0";
    const char *charSenderUUID = "ffc1";
    const char *charReceiverUUID = "ffc2";
    NimBLEClient *pClient;
    NimBLERemoteCharacteristic *pCharSender;
    NimBLERemoteCharacteristic *pCharReceiver;

  public:
    bool connect();
    bool isConnected();
    void disconnect();
    void setPreset(int presetNum);
    void setDrive(bool active);
    void getCurrentPresetInfo();
};

#endif