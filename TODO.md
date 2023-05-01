# TO-DO List

This is a list of ideas, tweaks or fixes that are still pending in the project.

### Todo

* [ ] Add the fifth footswitch to the code
* [ ] Turn the PoC into something more organized and better structured (move code to modules)
* [X] Add a software debounce algorithm for GPIO footswitch inputs
* [X] Add hibernation to turn the board off and preserve battery: https://lastminuteengineers.com/esp32-sleep-modes-power-consumption/
  * [X] Add a power led indicator
  * [X] Turn it off by long-pressing footswitch 4 and 5
  * [X] Turn it on by short-pressing footswitch 1 and 5
  * [X] Turn it off automatically if Bluetooth is disconnected for more than 5 minutes
* [X] Add a blue led for Bluetooth connection status (blinking when connected, on when not connected)
* [ ] Measure the battery level (GPIO 35 in Lolin D32 board)
  * [ ] Power led flashes fast when battery is low
  * [ ] Power led flashes slowly when charging
  * [ ] Power led keeps on when the charging is completed
  
