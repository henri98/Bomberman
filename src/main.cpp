//#include <QueueLib.h>
#include <IRCommLib.h>
#include <Arduino.h>


int main()
{

  /* set pin 5 of PORTB for output*/
  DDRB |= _BV(DDB5);

  initIRCommLib();
  Serial.begin(250000);

  while (1)
  {

    if (Serial.available() > 0)
    {
      // read the incoming byte:
      unsigned char incomingByte = Serial.read();
      // Serial.println(incomingByte);
      sendByte(&incomingByte);

    }
  }
}
