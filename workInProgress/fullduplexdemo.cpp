#include "ir_lib.h"
#include "Arduino.h"

int main(int argc, char const * argv[])
{
    /* code */
    /* set pin 5 of PORTB for output*/
    DDRB |= _BV(DDB5);

    Serial.begin(250000);
    init_timer0();
    init_ir_sender();
    init_ir_receiver();
    while (1) {
        if (Serial.available() > 0) {
            // read the incoming byte:
            char incomingByte = Serial.read();
            byte_to_send(incomingByte);
        }
    }
    return 0;
}

