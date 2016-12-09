#include <Arduino.h>
#include <MI0283QT9.h>
#include <game.h>
#include <IRCommLib.h>
MI0283QT9 lcd;
Player *player1;

int main()
{
  //Serial.begin(250000);
  initIRCommLib();
  init_display(lcd);
  load_map(lcd);

  nunchuck_init();
  player1 = (Player *)malloc(sizeof(Player));
  generate_map();
  init_player(player1, 2, 2, 0, 3, RGB(150,100,250));
  draw_player(player1,lcd);
  gameloop(player1, lcd);
  return 0;
}
