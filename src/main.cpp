#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

volatile unsigned long teller                 = 0;
volatile unsigned char byteToSend             = 5;
volatile unsigned char bitCounterSend         = 0x00;
volatile unsigned char delayTimeSend          = 0x00;
volatile unsigned char previousTimeSignalSend = 0x00;
volatile unsigned char highSignalEndSend      = 0x00;
volatile unsigned char transferStardedSend    = 0x00;


ISR(TIMER0_COMPA_vect)
{
    teller++;
    if (teller - previousTimeSignalSend >= delayTimeSend) {
        if (highSignalEndSend) {
            TCCR2A       &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
            delayTimeSend = 4;
            previousTimeSignalSend = teller;
            highSignalEndSend      = 0x00;
            //  Serial.println("Stop signal");
        } else {
            if (transferStardedSend) {
                if (bitCounterSend == 8) {
                    // Stopsignal
                    // Serial.println("Start Signal");
                    // Serial.println("stop");
                    TCCR2A       |= _BV(COM2B1); // Enable pin 3 PWM output
                    delayTimeSend = 25;
                    previousTimeSignalSend = teller;
                    highSignalEndSend      = 0x01;
                    transferStardedSend    = 0x00;
                    bitCounterSend         = 0x00;
                } else {
                    if ((byteToSend >> bitCounterSend) & 1) {
                        // Signal 1
                        // Serial.println("Start Signal");
                        // Serial.println("1");
                        TCCR2A       |= _BV(COM2B1); // Enable pin 3 PWM output
                        delayTimeSend = 8;
                        previousTimeSignalSend = teller;
                        highSignalEndSend      = 0x01;
                        bitCounterSend++;
                    } else {
                        // Signal 0
                        // Serial.println("0");
                        TCCR2A       |= _BV(COM2B1); // Enable pin 3 PWM output
                        delayTimeSend = 4;
                        previousTimeSignalSend = teller;
                        highSignalEndSend      = 0x01;
                        bitCounterSend++;
                    }
                }
            }  else {
                if (teller % 5000 == 1) {
                    // Startsignal
                    //  Serial.println("Start Signal");
                    //  Serial.println("Start");
                    TCCR2A       |= _BV(COM2B1); // Enable pin 3 PWM output
                    delayTimeSend = 35;
                    previousTimeSignalSend = teller;
                    transferStardedSend    = 0x01;
                    highSignalEndSend      = 0x01;
                    bitCounterSend         = 0x00;
                }
            }
        }
    }
}

void init_ir_transmitter()
{
    /* set pin 3 of PORTD for output*/
    DDRD  |= (1 << DDD3);
    PORTD &= ~(1 << PORTD3);

    // WGM2 = 101: phase-correct PWM with OCRA as top
    // CS2 = 000: no prescaling
    TCCR2A = _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS20);

    // frequency and duty cycle
    int khz = 38;
    OCR2A = F_CPU / 2 / khz / 1000;
    OCR2B = OCR2A / 3; // 33% duty cycle
}

// init Timer0
void init_timer()
{
    cli();
    TCCR0A |= (1 << WGM01);
    // Set the value that you want to count to
    OCR0A   = 25;
    TIMSK0 |= (1 << OCIE0A); // Set the ISR COMPA vect
    sei();                   // enable interrupts

    TCCR0B = (1 << CS01) | (1 << CS00);
}

// temp main for testing
int main(int argc, char const * argv[])
{
    Serial.begin(9600);
    init_ir_transmitter();
    init_timer();
    sei();
    while (1) { }
}
