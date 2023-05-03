#include "globals.h"
#include "battery.h"
#include "sparkcomms.h"
#include <Bounce2.h>
#include <Arduino.h>

// CONSTANTS
const int numFootswitch = sizeof(footswitchGPIO) / sizeof(footswitchGPIO[0]);
const int numWakeUp = sizeof(wakeUpGPIO) / sizeof(wakeUpGPIO[0]);
const int powerLedBlinkInterval = 500; //milliseconds

// GLOBAL VARS
Bounce2::Button buttons[numFootswitch];
SparkMiniComms miniComms;
int powerLedLastBlink = millis();
int powerLedStatus = !POWER_LED_ACTIVE;
int lastConnectionTime = 0;
int connectionLastAttempt = 0 - CONNECTION_TIME_BETWEEN_ATTEMPTS_MS;
bool driveActive = true;


// SETUP

void setupPowerLed() {
  pinMode(powerLedGPIO, OUTPUT);
  powerLedStatus = POWER_LED_ACTIVE;
  digitalWrite(powerLedGPIO, powerLedStatus);
  powerLedLastBlink = millis();
}

void setupButtons() {
  for (int i=0; i<numFootswitch; i++) {
    buttons[i] = Bounce2::Button();
    buttons[i].attach(footswitchGPIO[i], INPUT_PULLUP);
    buttons[i].interval(DEBOUNCE_INTERVAL_MS);
    buttons[i].setPressedState(BUTTON_ACTIVE); 
  }
}

void connectToSpark() {
  Serial.println("Connecting to Spark Mini...");
  powerLedStatus = POWER_LED_ACTIVE;
  digitalWrite(powerLedGPIO, powerLedStatus);
  miniComms.connect();
  if (miniComms.isConnected()){
    Serial.println("Connected!");
  } else { 
    Serial.println("Spark Mini not found!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Battery voltage: %.2f\n", getBatteryLevel());
  setupPowerLed();
  setupButtons();
}


// MAIN LOGIC

void goDeepSleep() {
  Serial.println("Going to deep sleep!");
  delay(100);

  // Required to keep internal pull-up resistors powered up while in deep sleep
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

  // Shutdown other components to reduce power consumption in sleep mode
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

  // pull-up and pull-down resistors configuration for wake-up GPIO inputs
  for (int i=0; i<numWakeUp; i++) {
    pinMode(wakeUpGPIO[i], INPUT_PULLUP);
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

  // Power led blink logic
  if ((miniComms.isConnected()) && (millis() - powerLedLastBlink > powerLedBlinkInterval)) {
    powerLedStatus = !powerLedStatus;
    digitalWrite(powerLedGPIO, powerLedStatus);
    powerLedLastBlink = millis();
  }

  // Main logic
  if (miniComms.isConnected()) {
    lastConnectionTime = millis();
    // Buttons and comms logic
    for (int i=0; i<numFootswitch; i++) {
      buttons[i].update();
      //if (buttons[i].pressed()) {
      if (buttons[i].getPressedState() == HIGH ? buttons[i].fell() : buttons[i].rose()) {
        if (i <= 3) {
          Serial.printf("Change to preset number %i\n", (i+1));
          miniComms.setPreset(i);
        } else if (i == 4) {
          Serial.printf("Pedal toggle\n", (i+1));
          driveActive = !driveActive;
          miniComms.setDrive(driveActive);
        } else {
          Serial.printf("Switch number %i not supported\n", (i+1));
        }
      }
    }
  } else {
    // Let's go sleep if max idle time has reached
    if ((millis() - lastConnectionTime) > CONNECTION_MAX_IDLE_TIME_MS) {
      goDeepSleep();
      return;
    }

    // Connection logic and retries
    if ((millis() - connectionLastAttempt) > CONNECTION_TIME_BETWEEN_ATTEMPTS_MS) {
      connectionLastAttempt = millis();
      connectToSpark();
    }
  }

  // Deep sleep logic
  if ( (buttons[0].isPressed()) && (buttons[0].currentDuration() > 3000) 
    && (buttons[1].isPressed()) && (buttons[1].currentDuration() > 3000) ) {
      goDeepSleep();
      return;
  }
}
