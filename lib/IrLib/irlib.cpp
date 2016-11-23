#include "irlib.h"

volatile unsigned long teller = 0;
volatile unsigned long time1 = 0;
volatile unsigned long time2 = 0;
volatile unsigned long length = 0;
volatile unsigned int receivedByte = 0x0;
volatile unsigned char transferStarted = 0x0;
volatile unsigned char bitCounter = 0x0;


//temp main for testing
int main(int argc, char const *argv[])
{
  Serial.begin(9600);
  DDRB |= (1 << DDB5);
  init_ir();
  init_timer();
  sei();
  while(1)
  {

  }
}

/*  When interrupt on PIN 11 --> receive IR */
ISR(PCINT0_vect)
{
    PORTB ^= (1 << PORTB5);
    if(time1 == 0)
    {
      time1 = teller;
    } else
    {
      time2 = teller;
      length = time2 - time1;
      time1 = 0;
      time2 = 0;
      //Serial.println(length);
      if(length > 30 && length < 40)
      {
        //Serial.println("Start transfer bit.");
        receivedStart();
      }
      if(length > 20 && length < 30)
      {
        // Serial.println("\nStop transfer bit");
        receivedStop();
      }
      if(length > 7 && length < 13)
      {
      //  Serial.print("1");
        receivedBit(1);
      }
      if(length > 3 && length < 7)
      {
      //  Serial.print("0");
        receivedBit(0);
      }
    }
}

// Process a received bit
void receivedBit(unsigned char i)
{
  if(transferStarted)
  {
    if(i)
    {
      receivedByte |= 1 << bitCounter;
    } else
    {
      receivedByte |= 0 << bitCounter;
    }
  }
  bitCounter++;
}

// Process the start signal
void receivedStart()
{
    transferStarted = 0x01;
}

// Process the stop signal
void receivedStop()
{
    transferStarted = 0x0;
    if(bitCounter == 8){
      Serial.write(receivedByte);
    } else{
      Serial.println("Bit transfer failed!");
      Serial.println(bitCounter);
    }
    bitCounter = 0x0;
    receivedByte = 0x0;
}


ISR(TIMER0_COMPA_vect)
{
  teller++;
}

// init Timer0
void init_timer()
{
  cli();
  TCCR0A |= (1 << WGM01);
  // Set the value that you want to count to
  OCR0A = 25;
  TIMSK0 |= (1 << OCIE0A);    // Set the ISR COMPA vect
  sei();         //enable interrupts


  TCCR0B = (1<<CS01) | (1<<CS00);

}

// init the IR receiver
void init_ir()
{
  cli();
  DDRB |= (0 << DDB3); // Enable PIN 11 as INPUT
  PORTB |= (1 << PORTB3); //Enable internal pulldown of PIN 11
  PCMSK0 |= (1<<PCINT3); //Choose PCINT3 to Interrupt
  PCICR |= (1<<PCIE0); //Turn on interrupts on port B (PCINT0 – PCINT7)
  sei();
}
