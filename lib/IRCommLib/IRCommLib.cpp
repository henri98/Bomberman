#include "IRCommLib.h"

#define NONE 0x10
#define PLAYER_POS 0x11
#define BOMB_PLACED 0x12
#define BOMB_EXPLODED 0x13
#define SCORE 0x14

#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[0] ))

uint8_t readingType = NONE;
uint8_t readingPos = 0;

Queue *receivedBytesQueue;
Queue *sendBytesQueue;

OpponentPos upToDateOpponentPos;
OpponentBombPos receivedBombs[3];
uint8_t receivingBombID;

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

void sendBombExploded(unsigned char ID)
{
  unsigned char type = BOMB_EXPLODED;
  sendByte(type);
  sendByte(ID);
}

void sendScore(unsigned char score)
{
  unsigned char type = SCORE;
  sendByte(type);
  sendByte(score);
}

void byteWasReceived()
{
  unsigned char *byte = &Dequeue(receivedBytesQueue)->data.byte;

  switch (readingType)
    {
    case NONE:
    {
      if (*byte == PLAYER_POS || *byte == BOMB_PLACED || *byte == BOMB_EXPLODED || *byte == SCORE)
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
        }
      else if (readingPos == 1)
        {
          receivedBombs[receivingBombID].id = (unsigned int) *byte;
        }
      else if (readingPos == 2)
        {
          receivedBombs[receivingBombID].location_x = (unsigned int) *byte;
        }
      else if (readingPos == 3)
        {
          receivedBombs[receivingBombID].location_y = (unsigned int) *byte;
          readingType = NONE;
          readingPos = 0;
        }

      readingPos++;
      break;

    case BOMB_EXPLODED:
      if (readingPos == 0)
        {
          Serial.print("BombExplodedID: ");
          Serial.write(*byte);
          Serial.println();

          readingType = NONE;
          readingPos = 0;
        }

      readingPos++;
      break;

    case SCORE:
      if (readingPos == 0)
        {
          Serial.print("Opponent's score: ");
          Serial.write(*byte);
          Serial.println();

          readingType = NONE;
          readingPos = 0;
        }

      readingPos++;
      break;

    default:
      Serial.println("Nope, don't send this ...");
      break;
    }

  free(byte);
}
