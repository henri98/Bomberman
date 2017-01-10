#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "QueueLib.h"
#include <Arduino.h>

/* This header file defines all of the signal lengths. */

#define START_SIGNAL 35
#define STOP_SIGNAL  25
#define HIGH_BIT     8
#define LOW_BIT      4
#define KHZ          38
#define NO_SIGNAL    4
#define QUEUE_LENGTH 30

void init_timer1();
void init_ir_receiver(Queue *bytesReceived, void (*callback)(void));
void init_ir_sender(uint8_t wire, Queue *bytesToSend);
void tryToDequeueByte();
void EnqueueReceivedByte(unsigned char byte);
