#include <stdio.h>
#include <stdlib.h>
#include <MI0283QT9.h>
#include <SPI.h>
#include <GraphicsLib.h>
#include <Arduino.h>
#include <Nunchuck.h>
#include <IRCommLib.h>
#include <sprites.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

typedef struct {
  unsigned char location_x;
  unsigned char location_y;
  unsigned long time_placed;
  unsigned char exploded;
  unsigned char explosion_removed;
} Bomb;

typedef struct {
  unsigned char location_x;
  unsigned char location_y;
  unsigned int points;
  unsigned int lifes;
  uint_least16_t color;
  Bomb bomblist[4];
} Player;


void initinit();
void generate_map();
void init_player(Player *player1,int x,int y,int points, int lifes,  uint_least16_t color);
void load_map(MI0283QT9 lcd);
void init_display(MI0283QT9 lcd);
void draw_player(Player *player, MI0283QT9 lcd);
void move_left(Player *player, MI0283QT9 lcd);
void move_right(Player *player,MI0283QT9 lcd);
void move_down(Player *player,MI0283QT9 lcd);
void move_up(Player *player,MI0283QT9 lcd);
void place_bomb(Player *player);
void draw_bomb(Player *player, MI0283QT9 lcd);
void gameloop(Player *player, Player *opponent, MI0283QT9 lcd);
void draw_lifes(Player *player, MI0283QT9 lcd, int opponent);
void check_if_player_in_bomb_explosion();
void updateOpponent();
