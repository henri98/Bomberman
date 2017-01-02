#include <ir_lib.h>

void initIRCommLib();
void sendByte(unsigned char *byte);

void byteWasReceived();

void sendPlayerPos(unsigned char x, unsigned char y);
void sendBombPlaced(unsigned char x, unsigned char y, unsigned char ID);
void sendScore(unsigned char score);
void sendSeed(unsigned char seed);

typedef struct {
  unsigned int location_x;
  unsigned int location_y;
} OpponentPos;

typedef struct {
  uint8_t location_x;
  uint8_t location_y;
  uint8_t is_used;
  uint8_t id;
} OpponentBombPos;

extern OpponentPos upToDateOpponentPos;
extern OpponentBombPos receivedBombs[3];
extern uint8_t receivingBombID;

extern uint8_t up_to_date_seed;
extern uint8_t received_seed;

void setOpponentPos(unsigned int x, unsigned int y);
