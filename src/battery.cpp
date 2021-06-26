#include "battery.h"
#include <Arduino.h>

float getBatteryLevel() {
  return analogRead(BATTERY_GPIO) / BOARD_ANALOG_INPUT_RANGE * BOARD_NOMINAL_VOLTAGE * INPUT_VOLTAGE_DIVIDER * INPUT_CORRECTION_FACTOR; 
}
