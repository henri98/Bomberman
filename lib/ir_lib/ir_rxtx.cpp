#include "ir_lib.h"

// count every 10 microseconds
volatile unsigned long counter = 0;

// transmit
Queue *bytesToSendQueue;
volatile unsigned char send_signal        = 0;
volatile unsigned long send_pulse_width   = 0;
volatile unsigned long send_pulse_started = 0;
volatile unsigned char send_byte = 'a'; // byte to send
volatile unsigned char current_byte_send = 0;

// This can store a pointer to a function.
// This can be called from here which will be executed somewhere else.
// Using this mechanism it can notify something else that a byte is received.
typedef void (*receivedCallback)(void);
receivedCallback callbackFunc;

// receive
Queue *bytesReceivedQueue;
volatile unsigned long receive_pulse_started    = 0;
volatile unsigned long receive_pulse_stoped     = 0;
volatile unsigned long receive_signal_width     = 0;
volatile unsigned char receive_transfer_started = 0x0;
volatile unsigned char receive_bit_counter      = 0x0;
volatile unsigned char receive_byte = 0x0; // byte to receive

/**
 * This will try to dequeue a byte, when it isn't doing anything.
 */
void tryToDequeueByte()
{
  if (current_byte_send == 0 && !isEmpty(bytesToSendQueue))
    {
      // Dequeue the node to send
      NODE *nodeToSend = (NODE *)malloc(sizeof(NODE));
      nodeToSend = Dequeue(bytesToSendQueue);

      // Get the byte to send from the node
      send_byte = nodeToSend->data.byte;
      current_byte_send = 1;

      free(nodeToSend);
    }
}

/**
 * Add the received byte to the received-queue
 * @param byte The byte that was received.
 */
void EnqueueReceivedByte(unsigned char byte)
{
  NODE *newNode = (NODE *)malloc(sizeof(NODE));
  newNode->data.byte = byte;

  Enqueue(bytesReceivedQueue, newNode);

  // Call the callback-function, to notify something else of the queued byte.
  callbackFunc();
}

//  Function to init Timer0
void init_timer0()
{
  cli();
  TCCR0A |= (1 << WGM01);
  OCR0A   = 25;
  TIMSK0 |= (1 << OCIE0A);
  sei();
  TCCR0B = (1 << CS01) | (1 << CS00);
}

/**
 * Initialise the receiver.
 * @param bytesReceived The queue to enqueue the received bytes to.
 * @param callback      The function that should be called when a byte is queued.
 */
void init_ir_receiver(Queue *bytesReceived, void (*callback)(void))
{
  callbackFunc = callback;
  bytesReceivedQueue = bytesReceived;

  cli();
  DDRB   |= (0 << DDB3);
  PORTB  |= (1 << PORTB3);
  PCMSK0 |= (1 << PCINT3);
  PCICR  |= (1 << PCIE0);
  sei();
}

/**
 * Initialise the sender.
 * @param wire              Defines if IR or Wires will be used.
 * @param bytesToSendQueue  The queue that contains bytes to send.
 */
void init_ir_sender(uint8_t wire, Queue *bytesToSend)
{
  bytesToSendQueue = bytesToSend;

  // @TODO: Replace this with AVR-code !
  pinMode(3, OUTPUT);     // enable pin 3 as output for ir led //ARDUINO.H
  digitalWrite(3, LOW);   // When not sending PWM, we want it low //ARDUINO.H


  // prescaling
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);

  //enable overflow interrupt on timer two
  TIMSK2 = _BV(TOIE2);

  // Only set the 38KHz when communicating over IR.
  if (wire == 0)
    {
      // frequency and duty cycle
      OCR2A = F_CPU / 2 / KHZ / 1000;
      OCR2B = OCR2A / 3;     // 33% duty cycle
    }
}

ISR(TIMER0_COMPA_vect)
{
  counter++;
  if (current_byte_send || send_signal)
    {
      // send data
      if (counter - send_pulse_started >= send_pulse_width)
        {
          if (send_signal % 2)
            {
              TCCR2A &= ~(_BV(COM2B1));   // Disable pin 3 PWM output --> turn off IR led
              send_pulse_started = counter;
              send_pulse_width   = NO_SIGNAL;
              send_signal++;
            }
          else
            {
              if (send_signal == 0)
                {
                  // start signal
                  TCCR2A |= (_BV(COM2B1));   // Enable pin 3 PWM output --> turn on IR led
                  send_pulse_started = counter;
                  send_pulse_width   = START_SIGNAL;
                  send_signal++;
                  // current_byte_send = 0;
                }
              else if (send_signal == 18)
                {
                  // stop signal
                  TCCR2A |= (_BV(COM2B1));   // Enable pin 3 PWM output --> turn on IR led
                  send_pulse_started = counter;
                  send_pulse_width   = STOP_SIGNAL;
                  send_signal++;
                }
              else if (send_signal == 20)
                {
                  send_signal         = 0;
                  current_byte_send = 0;
                  tryToDequeueByte();
                }
              else
                {
                  // data bit
                  TCCR2A |= (_BV(COM2B1));   // Enable pin 3 PWM output --> turn on IR led

                  if ((send_byte >> ((send_signal / 2) - 1)) & 1)
                    {
                      send_pulse_width = HIGH_BIT;
                    }
                  else
                    {
                      send_pulse_width = LOW_BIT;
                    }

                  send_pulse_started = counter;
                  send_signal++;
                }
            }
        }
    }
}

// When interrupt on PIN 11 --> receive IR
ISR(PCINT0_vect)
{
  if (receive_pulse_started == 0)
    {
      receive_pulse_started = counter;
    }
  else
    {
      receive_pulse_stoped  = counter;
      receive_signal_width  = receive_pulse_stoped - receive_pulse_started;
      receive_pulse_started = 0;
      receive_pulse_stoped  = 0;

      if (receive_signal_width > (START_SIGNAL - 5) && receive_signal_width < (START_SIGNAL + 5))
        {
          // start
          receive_transfer_started = 0x01;
        }
      if (receive_signal_width > (STOP_SIGNAL - 5) && receive_signal_width < (STOP_SIGNAL + 5))
        {
          // stop
          EnqueueReceivedByte((unsigned char)receive_byte);

          receive_transfer_started = 0x0;
          receive_bit_counter      = 0x0;
          receive_byte = 0x0;
        }
      if (receive_signal_width > (HIGH_BIT - 2) && receive_signal_width < (HIGH_BIT + 2))
        {
          if (receive_transfer_started)
            {
              receive_byte |= 1 << receive_bit_counter;
              receive_bit_counter++;
            }
        }
      if (receive_signal_width > (LOW_BIT - 2) && receive_signal_width < (LOW_BIT + 2))
        {
          if (receive_transfer_started)
            {
              receive_byte |= 0 << receive_bit_counter;
              receive_bit_counter++;
            }
        }
    }
}
