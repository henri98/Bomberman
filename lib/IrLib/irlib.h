#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Arduino.h>

// init the timer
void init_timer();

//init ir
void init_ir();

//if received start signal
void receivedStart();

//if received stop signal
void receivedStop();

//if received a bit
void receivedBit(unsigned char i);
