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

/* Initialise registers timer 0 */
void init_timer0();
void init_ir_receiver();
void init_ir_sender(uint8_t wire, Queue *bytesToSend);
void byte_to_send(unsigned char c);
void notifyByteAdded();
/* struct with all the status parameters */
// typedef struct
// {
//    int *send_signal;
//    int *send_pulse_width;
//    int *send_pulse_started;
// } Status;
