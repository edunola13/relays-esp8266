#include <Arduino.h>

void loadConfigModules(uint8_t pos) {
  // Leave space between
  pos = RELAY_CONFIG_POSITION;
  // Put the last state
  bool isOpen = EEPROM.read(pos++);
  if (isOpen) {
    relays[0].commonlyOpen();
  }
  isOpen = EEPROM.read(pos++);
  if (isOpen) {
    relays[1].commonlyOpen();
  }
  isOpen = EEPROM.read(pos++);
  if (isOpen) {
    relays[2].commonlyOpen();
  }
  isOpen = EEPROM.read(pos++);
  if (isOpen) {
    relays[3].commonlyOpen();
  }
}

void saveConfigModules(uint8_t pos) {
  // Leave space between
  pos = RELAY_CONFIG_POSITION;
  // Store actual state
  EEPROM.put(pos++, relays[0].isOpen());
  EEPROM.put(pos++, relays[1].isOpen());
  EEPROM.put(pos++, relays[2].isOpen());
  EEPROM.put(pos++, relays[3].isOpen());
}
