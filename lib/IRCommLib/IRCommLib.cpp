#include "IRCommLib.h"

// TODO: Dit wordt nu niet gebruikt, staat hardcoded in elke functie. Aanpassen dus!!!
#define NONE 0x10
#define PLAYER_POS 0x11
#define BOMB_PLACED 0x12
#define BOMB_EXPLODED 0x13
#define SCORE 0x14

uint8_t readingType = NONE;
uint8_t readingPos = 0;

Queue *receivedBytesQueue;
Queue *sendBytesQueue;

Opponent *opponentPos;

void initIRCommLib()
{
  init_timer0();
  receivedBytesQueue = ConstructQueue(6000);
  sendBytesQueue = ConstructQueue(6000);

  // Send a pointer to the function byteWasReceived as argument.
  // This can now be called by the ir_lib.
  init_ir_receiver(receivedBytesQueue, &byteWasReceived);
  init_ir_sender(0, sendBytesQueue);

  opponentPos = (Opponent *)malloc(sizeof(Opponent));
}

void sendByte(unsigned char *byte)
{
  NODE *newNode = (NODE *)malloc(sizeof(NODE));
  newNode->data.byte = *byte;

  Enqueue(sendBytesQueue, newNode);
  tryToDequeueByte();
}

unsigned char getOpponentPosX()
{
  return opponentPos->location_x;
}

unsigned char getOpponentPosY()
{
  return opponentPos->location_y;
}

void sendPlayerPos(unsigned char *x, unsigned char *y)
{
  unsigned char type = PLAYER_POS;
  sendByte(&type);
  sendByte(x);
  sendByte(y);
}

void sendBombPlaced(unsigned char *x, unsigned char *y, unsigned char *ID)
{
  unsigned char type = BOMB_PLACED;
  sendByte(&type);
  sendByte(x);
  sendByte(y);
  sendByte(ID);
}

void sendBombExploded(unsigned char *ID)
{
  unsigned char type = BOMB_EXPLODED;
  sendByte(&type);
  sendByte(ID);
}

void sendScore(unsigned char *score)
{
  unsigned char type = SCORE;
  sendByte(&type);
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
      opponentPos->location_x = *byte;
      // Serial.print("PlayerX: ");
      // Serial.write(*byte);
      // Serial.println();
    }
    else if (readingPos == 1)
    {
      opponentPos->location_y = *byte;
      // Serial.print("PlayerY: ");
      // Serial.write(*byte);
      // Serial.println();

      readingType = NONE;
      readingPos = 0;
    }

    readingPos++;
    break;

  case BOMB_PLACED:
    if (readingPos == 0)
    {
      Serial.print("BombPlacedX: ");
      Serial.write(*byte);
      Serial.println();
    }
    else if (readingPos == 1)
    {
      Serial.print("BombPlacedY: ");
      Serial.write(*byte);
      Serial.println();
    }
    else if (readingPos == 2)
    {
      Serial.print("BombPlacedID: ");
      Serial.write(*byte);
      Serial.println();

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
