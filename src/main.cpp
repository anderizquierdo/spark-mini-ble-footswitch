#include "battery.h"
#include <BLEMidi.h>
#include <Bounce2.h>
#include <Arduino.h>

#define DEBOUNCE_INTERVAL_MS 20

// CONSTANTS
const gpio_num_t powerLedGPIO = GPIO_NUM_13; 
const gpio_num_t footswitchGPIO[] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27};
const gpio_num_t wakeUpGPIO[] = {GPIO_NUM_32, GPIO_NUM_27};
const int numFootswitch = sizeof(footswitchGPIO) / sizeof(footswitchGPIO[0]);
const int numWakeUp = sizeof(wakeUpGPIO) / sizeof(wakeUpGPIO[0]);

// GLOBAL VARS
Bounce2::Button buttons[numFootswitch];


// SETUP

void setupPowerLed() {
  pinMode(powerLedGPIO, OUTPUT);
  digitalWrite(powerLedGPIO, HIGH);
}

void setupButtons() {
  for (int i=0; i<numFootswitch; i++) {
    buttons[i] = Bounce2::Button();
    buttons[i].attach(footswitchGPIO[i], INPUT_PULLUP);
    buttons[i].interval(DEBOUNCE_INTERVAL_MS);
    buttons[i].setPressedState(LOW); 
  }
}

void onMidiConnected() {
  Serial.println("MIDI device connected");
}

void onMidiDisconnected() {
  Serial.println("MIDI device disconnected");
}

void setupBluetooth() {
  Serial.println("Initialize Bluetooth");
  BLEMidiServer.begin("Yamaha THR Pedalboard");
  BLEMidiServer.setOnConnectCallback(onMidiConnected);
  BLEMidiServer.setOnDisconnectCallback(onMidiDisconnected);
  Serial.println("Waiting for connections...");
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Battery voltage: %.2f\n", getBatteryLevel());
  setupPowerLed();
  setupButtons();
  setupBluetooth();
}


// MAIN LOGIC

void goDeepSleep() {
  // Required to keep internal pull-up resistors powered up while in deep sleep
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

  // Shutdown other components to reduce power consumption in sleep mode
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

  // pull-up and pull-down resistors configuration for wake-up GPIO inputs
  for (int i=0; i<numWakeUp; i++) {
    gpio_pulldown_dis(wakeUpGPIO[i]);
    gpio_pullup_en(wakeUpGPIO[i]);
  }

  // Calculate wake-up GPIO mask (see ESP32 documentation for more details)
  uint64_t wakeUpGPIOMask = 0;
  for (int i=0; i<numWakeUp; i++) {
    wakeUpGPIOMask += (uint64_t)1 << wakeUpGPIO[i];
  }
  
  // Go to deep sleep
  esp_sleep_enable_ext1_wakeup(wakeUpGPIOMask, ESP_EXT1_WAKEUP_ALL_LOW);
  esp_deep_sleep_start();
}

// Main loop, quite messy for now
void loop() {
  // Buttons and bluetooth logic
  for (int i=0; i<numFootswitch; i++) {
    buttons[i].update();
    if (buttons[i].pressed()) {
      Serial.printf("Switch number %i ON\n", (i+1));
      if (BLEMidiServer.isConnected()) {
        BLEMidiServer.controlChange(0, 40 + i, 127);
      }
    }
  }

  // Deep sleep logic
  if ( (buttons[numFootswitch - 2].read() == LOW) && (buttons[numFootswitch - 2].duration() > 3000) 
    && (buttons[numFootswitch - 1].read() == LOW) && (buttons[numFootswitch - 1].duration() > 3000) ) {
      Serial.println("Going to deep sleep!");
      delay(100);
      goDeepSleep();
  }
}
