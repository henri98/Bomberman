#include <mem.h>

//This function will write a uint16_t  to the eeprom at
//the specified address to address + 1
void EEPROM_write_uint16_t(int address, uint16_t value)
{
  //Decomposition from a uint16_t to 2 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  char two = (value & 0xFF);
  char one = ((value >> 8) & 0xFF);
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
}

//This function will return a 2 byte uint16_t from the eeprom
//at the specified address to address + 1.
uint16_t EEPROM_read_uint16_t(int address)
{
  //Read the 4 bytes from the eeprom memory.
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);
  //Return the recomposed long by using bitshift.
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

void save_score(uint16_t value)
{
  uint16_t scorelist[5];
  for (size_t i = 0; i < 5; i++)
    {
      scorelist[i] = EEPROM_read_uint16_t(0 + (i * 2));
    }
  for (size_t i = 5; i > 0; i--)
    {
      scorelist[i] = scorelist[i-1];
    }
  scorelist[0] = value;
  for (size_t i = 0; i < 5; i++)
    {
      EEPROM_write_uint16_t((i * 2), scorelist[i]);
    }
}
