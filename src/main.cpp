#include <Arduino.h>
#include <MI0283QT9.h>
#include <game.h>
#include "ir_lib.h"

MI0283QT9 lcd;
Player *player1;


int main()
{
  DDRB |= _BV(DDB5);
  init_timer0();
  init_ir_sender();
//  init_ir_receiver();
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
