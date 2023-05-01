#define DEBOUNCE_INTERVAL_MS 20
#define POWER_LED_ACTIVE LOW
#define BUTTON_ACTIVE LOW
#define CONNECTION_TIME_BETWEEN_ATTEMPTS_MS 15000
#define CONNECTION_MAX_IDLE_TIME_MS 300000

const int footswitchGPIO[] = {32, 33, 25, 26, 27};
const int powerLedGPIO = 5; 
const int wakeUpGPIO[] = {32, 27};