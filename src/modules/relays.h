#ifndef ModuleActuators_h
#define ModuleActuators_h

#include <actuators/Relay.h>

Relay relays[] = {Relay(D6, true), Relay(D2, true), Relay(D1, true), Relay(D0, true)};

void save_status_relays() {
  uint8_t pos = RELAY_CONFIG_POSITION;
  // Store actual state
  EEPROM.begin(512);
  EEPROM.put(pos++, relays[0].isOpen());
  EEPROM.put(pos++, relays[1].isOpen());
  EEPROM.put(pos++, relays[2].isOpen());
  EEPROM.put(pos++, relays[3].isOpen());
  EEPROM.end();
}

void change_status_relay(Relay &relay, bool on) {
  if (on) {
    relay.commonlyOpen();
  } else {
    relay.commonlyClose();
  }

  save_status_relays();
}

#endif
