#ifndef BATTERY_H
#define BATTERY_H

// Lolin D32 has the VBat internally connected to this GPIO
#define BATTERY_GPIO 35

// Value range in Lolin D32 for analog input is [0, 4095], so 4096 possible values
#define BOARD_ANALOG_INPUT_RANGE 4096.0

// Nominal voltage in Lolin D32 is 3.3v, so this is the maximum input level in the GPIO
#define BOARD_NOMINAL_VOLTAGE 3.3 

// Lolin D32 has an internal VBat/2 divider, for other boards or external dividers, set the proper value here
#define INPUT_VOLTAGE_DIVIDER 2 

// Empirical correction factor based on several measurements using a multimeter
#define INPUT_CORRECTION_FACTOR 1.03

float getBatteryLevel();

#endif