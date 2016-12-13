#include <Arduino.h>
#include <MI0283QT9.h>
#include <game.h>
MI0283QT9 lcd;
Player *player1;
Player *oponent;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void initinit()
{
  // this needs to be called before setup() or some functions won't
  // work there
  sei();

  // on the ATmega168, timer 0 is also used for fast hardware pwm
  // (using phase-correct PWM would mean that timer 0 overflowed half as often
  // resulting in different millis() behavior on the ATmega8 and ATmega168)
  #if defined(TCCR0A) && defined(WGM01)
  sbi(TCCR0A, WGM01);
  sbi(TCCR0A, WGM00);
  #endif

  // set timer 0 prescale factor to 64
  #if defined(__AVR_ATmega128__)
  // CPU specific: different values for the ATmega128
  sbi(TCCR0, CS02);
  #elif defined(TCCR0) && defined(CS01) && defined(CS00)
  // this combination is for the standard atmega8
  sbi(TCCR0, CS01);
  sbi(TCCR0, CS00);
  #elif defined(TCCR0B) && defined(CS01) && defined(CS00)
  // this combination is for the standard 168/328/1280/2560
  sbi(TCCR0B, CS01);
  sbi(TCCR0B, CS00);
  #elif defined(TCCR0A) && defined(CS01) && defined(CS00)
  // this combination is for the __AVR_ATmega645__ series
  sbi(TCCR0A, CS01);
  sbi(TCCR0A, CS00);
  #else
          #error Timer 0 prescale factor 64 not set correctly
  #endif

  // enable timer 0 overflow interrupt
  #if defined(TIMSK) && defined(TOIE0)
  sbi(TIMSK, TOIE0);
  #elif defined(TIMSK0) && defined(TOIE0)
  sbi(TIMSK0, TOIE0);
  #else
          #error  Timer 0 overflow interrupt not set correctly
  #endif
}

int main()
{
  initIRCommLib();
  initinit();
  Serial.begin(250000);
  init_display(lcd);
  generate_map();
  load_map(lcd);


  while (0)
  {
    if (Serial.available() > 0)
    {
      unsigned char byteToSend = (unsigned char)Serial.read();
      Serial.println(byteToSend);
      if (byteToSend == 'p')
      {
        unsigned char xPos = 0x32;
        unsigned char yPos = 0x33;
        sendPlayerPos(&xPos, &yPos);
      }
      else if (byteToSend == 'b')
      {
        unsigned char xPos = 0x31;
        unsigned char yPos = 0x35;
        unsigned char ID = 0x39;
        sendBombPlaced(&xPos, &yPos, &ID);
      }
      else if (byteToSend == 'e')
      {
        unsigned char ID = 0x39;
        sendBombExploded(&ID);
      }
      else if (byteToSend == 's')
      {
        unsigned char score = 0x30;
        sendScore(&score);
      }
    }
  }
  /// END TEMP CODE



  nunchuck_init();
  player1 = (Player *)malloc(sizeof(Player));
  oponent = (Player *)malloc(sizeof(Player));

  init_player(player1, 1, 1, 0, 3, RGB(150,100,250));
  init_player(oponent, 12, 10, 0, 3, RGB(44, 76, 23));
  draw_player(player1, lcd);
  draw_player(oponent, lcd);
  gameloop(player1, oponent, lcd);
  return 0;
}
