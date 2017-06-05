/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
#define ID_ADDRESS 0x00
#define CLIENT_ID 102

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  EEPROM.write(ID_ADDRESS, CLIENT_ID);
  Serial.println(EEPROM.read(ID_ADDRESS));
}

void loop() {
  /** Empty loop. **/
  delay(100);
}
