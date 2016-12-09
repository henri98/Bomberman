#include <Arduino.h>
#include <MI0283QT9.h>
#include <game.h>
#include <IRCommLib.h>
MI0283QT9 lcd;
Player *player1;

int main()
{
  Serial.begin(250000);
  initIRCommLib();


  /// TEMP CODE
  while (true)
  {
    if (Serial.available() > 0)
    {
      unsigned char byteToSend = (unsigned char)Serial.read();

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

  DDRB |= _BV(DDB5);
  nunchuck_init();
  player1 = (Player *)malloc(sizeof(Player));
  init_display(lcd);
  generate_map();
  load_map(lcd);
  init_player(player1, 2, 2, 0, 3, RGB(150,100,250));
  draw_player(player1,lcd);
  gameloop(player1, lcd);
  return 0;
}
