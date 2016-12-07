#include "IRCommLib.h"

Queue *receivedBytesQueue;
Queue *sendBytesQueue;

void initIRCommLib()
{
  init_timer0();
  receivedBytesQueue = ConstructQueue(6000);
  sendBytesQueue = ConstructQueue(6000);

  // Send a pointer to the function byteWasReceived as argument.
  // This can now be called by the ir_lib.
  init_ir_receiver(receivedBytesQueue, &byteWasReceived);
  init_ir_sender(1, sendBytesQueue);
}

void sendByte(unsigned char *byte)
{
  NODE *newNode = (NODE *)malloc(sizeof(NODE));
  newNode->data.byte = *byte;

  Enqueue(sendBytesQueue, newNode);
  tryToDequeueByte();
}

void byteWasReceived()
{
  unsigned char *byte = &Dequeue(receivedBytesQueue)->data.byte;
  Serial.print("Received byte: ");
  Serial.write(*byte);
  Serial.println();

  free(byte);
}
