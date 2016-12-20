#include <ir_lib.h>

void initIRCommLib();
void sendByte(unsigned char *byte);

void byteWasReceived();

void sendPlayerPos(unsigned char x, unsigned char y);
void sendBombPlaced(unsigned char x, unsigned char y, unsigned char ID);
void sendBombExploded(unsigned char ID);
void sendScore(unsigned char score);

typedef struct {
  unsigned int location_x;
  unsigned int location_y;
} OpponentPos;

void setOpponentPos(unsigned int x, unsigned int y);
OpponentPos getOpponentPos();
