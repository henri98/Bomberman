#include "IRCommLib.h"

#define NONE 0x10
#define PLAYER_POS 0x11
#define BOMB_PLACED 0x12
#define SCORE 0x13
#define SEED 0x14


#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[0] ))

uint8_t readingType = NONE;
uint8_t readingPos = 0;

Queue *receivedBytesQueue;
Queue *sendBytesQueue;

OpponentPos upToDateOpponentPos;
OpponentBombPos receivedBombs[3];
uint8_t receivingBombID;

uint8_t up_to_date_seed;
uint8_t received_seed = 0;

void setOpponentPos(unsigned int x, unsigned int y)
{
  upToDateOpponentPos.location_x = x;
  upToDateOpponentPos.location_y = y;
}

void initIRCommLib()
{
  // &upToDateOpponentPos = (OpponentPos *) malloc(sizeof(OpponentPos));

  init_timer0();
  receivedBytesQueue = ConstructQueue(75);
  sendBytesQueue = ConstructQueue(75);

  // Send a pointer to the function byteWasReceived as argument.
  // This can now be called by the ir_lib.
  init_ir_receiver(receivedBytesQueue, &byteWasReceived);
  init_ir_sender(1, sendBytesQueue);

  for (uint8_t i = 0; i < ARRAY_SIZE(receivedBombs); i++)
  {
    receivedBombs[i].location_x = 0;
    receivedBombs[i].location_y = 0;
    receivedBombs[i].is_used = 0;
  }
}

void sendByte(unsigned char byte)
{
  NODE *newNode = (NODE *)malloc(sizeof(NODE));
  newNode->data.byte = byte;

  Enqueue(sendBytesQueue, newNode);
  tryToDequeueByte();
}

void sendPlayerPos(unsigned char x, unsigned char y)
{
  unsigned char type = PLAYER_POS;
  sendByte(type);
  sendByte(x);
  sendByte(y);
}

void sendBombPlaced(unsigned char x, unsigned char y, unsigned char ID)
{
  unsigned char type = BOMB_PLACED;
  sendByte(type);
  sendByte(ID);
  sendByte(x);
  sendByte(y);
}

void sendScore(unsigned char score)
{
  unsigned char type = SCORE;
  sendByte(type);
  sendByte(score);
}

void sendSeed(unsigned char seed)
{
  unsigned char type = SEED;
  sendByte(type);
  sendByte(seed);
}

void byteWasReceived()
{
  unsigned char *byte = &Dequeue(receivedBytesQueue)->data.byte;

  switch (readingType)
  {
  case NONE:
  {
    if (*byte == PLAYER_POS || *byte == BOMB_PLACED || *byte == SCORE || *byte == SEED)
    {
      readingType = *byte;
      readingPos = 0;
    }
  }
  break;

  case PLAYER_POS:
    if (readingPos == 0)
    {
      upToDateOpponentPos.location_x = (unsigned int) *byte;
    }
    else if (readingPos == 1)
    {
      upToDateOpponentPos.location_y = (unsigned int) *byte;
      readingType = NONE;
      readingPos = 0;
    }

    readingPos++;
    break;

  case BOMB_PLACED:
    if (readingPos == 0)
    {
      receivingBombID = (unsigned int) *byte;
      receivedBombs[receivingBombID].id = (unsigned int) *byte;
    }
    else if (readingPos == 1)
    {
      receivedBombs[receivingBombID].location_x = (unsigned int) *byte;
    }
    else if (readingPos == 2)
    {
      receivedBombs[receivingBombID].location_y = (unsigned int) *byte;
      receivedBombs[receivingBombID].is_used = 0;
      readingType = NONE;
      readingPos = 0;
    }

    readingPos++;
    break;

  case SCORE:
    if (readingPos == 0)
    {
      readingType = NONE;
      readingPos = 0;
    }

    readingPos++;
    break;

  case SEED:
    if (readingPos == 0)
    {
      up_to_date_seed = *byte;
      received_seed = 1;
      readingType = NONE;
      readingPos = 0;
    }

    readingPos++;
    break;

  default:
    break;
  }

  free(byte);
}
