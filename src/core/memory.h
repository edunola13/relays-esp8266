#include <Arduino.h>

//
// INTERFACE
//
void loadConfigModules(uint8_t pos);
void saveConfigModules(uint8_t pos);

// CONFIG
void loadConfig() {
  EEPROM.begin(512);
  uint8_t load = 0;
  int pos = 0;
  EEPROM.get(0, load);
  pos = 1;
  if(load == 1) {
    EEPROM.get(pos, config);
    pos += sizeof(struct Config);

    //
    // SAVE THAT YOU NEED
    //
    loadConfigModules(pos);
  } else {
     DEB_DO_PRINTLN(NO_CONF);
  }
  DEB_DO_PRINTLN(OK_CONF);
  EEPROM.end();
}

void saveConfig() {
  int pos= 0;
  EEPROM.begin(512);

  EEPROM.put(pos++, 1);
  EEPROM.put(pos, config);
  pos += sizeof(struct Config);

  //
  // SAVE THAT YOU NEED
  //
  saveConfigModules(pos);

  EEPROM.commit();
  DEB_DO_PRINTLN(OK_SAVE_CONF);
}
