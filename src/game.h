#include <stdio.h>
#include <stdlib.h>
#include <MI0283QT9.h>
#include <SPI.h>
#include <GraphicsLib.h>
#include <Arduino.h>
#include <Nunchuck.h>


typedef struct {
  unsigned int location_x;
  unsigned int location_y;
  unsigned long time_placed;
} Bomb;

typedef struct {
  unsigned int location_x;
  unsigned int location_y;
  unsigned int points;
  unsigned int lives;
  uint_least16_t color;
  Bomb bomblist[4];
} Player;



void generate_map();
void init_player(Player *player1,int x,int y,int points, int lives,  uint_least16_t color);
void load_map(MI0283QT9 lcd);
void init_display(MI0283QT9 lcd);
void draw_player(Player *player,MI0283QT9 lcd);
void move_left(Player *player, MI0283QT9 lcd);
void move_right(Player *player,MI0283QT9 lcd);
void move_down(Player *player,MI0283QT9 lcd);
void move_up(Player *player,MI0283QT9 lcd);
void place_bomb(Player *player);
void draw_bomb(Player *player, MI0283QT9 lcd);
void gameloop(Player *player, MI0283QT9 lcd);
void wait(unsigned long delaytime);
