#include <ir_lib.h>

void initIRCommLib();
void sendByte(unsigned char *byte);

void byteWasReceived();

void sendPlayerPos(unsigned char x, unsigned char y);
void sendBombPlaced(unsigned char x, unsigned char y, unsigned char ID);
void sendBombExploded(unsigned char ID);
void sendScore(unsigned char score);
void sendSeed(unsigned char seed);

typedef struct {
  unsigned int location_x;
  unsigned int location_y;
} OpponentPos;

typedef struct {
  unsigned int location_x;
  unsigned int location_y;
  unsigned int is_used;
  unsigned int id;
} OpponentBombPos;

extern OpponentPos upToDateOpponentPos;
extern OpponentBombPos receivedBombs[3];
extern uint8_t receivingBombID;

extern uint8_t up_to_date_seed;
extern uint8_t received_seed;

void setOpponentPos(unsigned int x, unsigned int y);
