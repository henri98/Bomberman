#include <EEPROM.h>

void EEPROM_write_uint16_t(int address, uint16_t value);
uint16_t EEPROM_read_uint16_t(int address);
void save_score(uint16_t value);
