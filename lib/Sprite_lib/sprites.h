#include <stdio.h>
#include <stdlib.h>
#include <MI0283QT9.h>
#include <avr/pgmspace.h>

#define BLOCKSIZE         16


// for lighter color use:
// grey     ^
// grey1    | darker
// grey2    |
// grey3
// enz.

static uint_least16_t red = RGB(255, 0, 0);
static uint_least16_t green = RGB(0, 255, 0);
static uint_least16_t blue = RGB(0, 0, 255);
static uint_least16_t white = RGB(255, 255, 255);
static uint_least16_t white1 = RGB(248, 248, 248);
static uint_least16_t black = RGB(0, 0, 0);
static uint_least16_t grey = RGB(90, 90, 90);
static uint_least16_t grey1 = RGB(116, 116, 116);
static uint_least16_t orange = RGB(248, 120, 24);
static uint_least16_t yellow = RGB(248, 200, 48);
static uint_least16_t yellow1 = RGB(248, 232, 152);
static uint_least16_t background = RGB(103, 98, 96);

void draw_object(MI0283QT9 lcd, int x, int y, const char sprite[16][16]);
void draw_player_sprite(MI0283QT9 lcd, int x, int y, uint_least16_t c, const char sprite[24][16]);

const char life[16][16] PROGMEM =
{
  {'c', 'c', 'c', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'r', 'r', 'r', 'b', 'c', 'c', 'b', 'r', 'r', 'r', 'b', 'c', 'c'},
  {'c', 'b', 'r', 'r', 'w', 'r', 'r', 'b', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'b', 'r', 'r', 'w', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'w', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b'},
  {'c', 'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'c', 'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'c', 'c', 'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'c', 'c'},
  {'c', 'c', 'c', 'b', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'r', 'r', 'r', 'r', 'r', 'r', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'b', 'r', 'r', 'r', 'r', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'b', 'r', 'r', 'b', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c', 'c'}
};

const char wall[16][16] PROGMEM =
{
  {'#', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '#'},
  {'#', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@'},
  {'@', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@'},
  {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'@', '@', '#', '#', 'b', '@', '@', '@', '@', '@', '#', '#', 'b', '@', '@', '@'},
  {'@', '@', '@', '#', 'b', '@', '@', '@', '@', '@', '@', '#', 'b', '@', '@', '@'},
  {'@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@'},
  {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'#', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '#'},
  {'#', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@'},
  {'@', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@'},
  {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
  {'@', '@', '#', '#', 'b', '@', '@', '@', '@', '@', '#', '#', 'b', '@', '@', '@'},
  {'@', '@', '@', '#', 'b', '@', '@', '@', '@', '@', '@', '#', 'b', '@', '@', '@'},
  {'@', '@', '@', '@', 'b', '@', '@', '@', '@', '@', '@', '@', 'b', '@', '@', '@'},
  {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'}
};


const char wall3[16][16] PROGMEM =
{
  {'#', 'b', '@', '#', '#', '#', '#', 'b', 'b', '#', '#', '@', 'b', '@', '#', 'b'},
  {'#', 'b', '@', '@', '#', '#', '@', 'b', '@', '#', '@', 'b', '@', '@', '#', '#'},
  {'#', '@', 'b', '@', '@', '@', 'b', 'b', '@', '@', 'b', 'b', '@', '@', '@', '#'},
  {'@', '@', 'b', 'b', 'b', 'b', '#', 'b', 'b', 'b', 'b', 'b', 'b', 'b', '@', '@'},
  {'b', 'b', '@', '#', 'b', '#', '#', '@', 'b', '@', '#', '#', '#', 'b', 'b', 'b'},
  {'#', 'b', '@', '#', 'b', '@', '@', '@', 'b', '@', '#', '@', '@', 'b', '#', '#'},
  {'#', 'b', '@', '@', '#', 'b', 'b', 'b', 'b', '@', '@', 'b', 'b', 'b', '@', '#'},
  {'@', 'b', 'b', '@', '@', 'b', '@', '#', '#', 'b', 'b', '@', '#', 'b', '@', '@'},
  {'b', '#', '#', 'b', 'b', '@', '#', '#', '#', '#', 'b', '@', '#', '#', 'b', '@'},
  {'@', '#', '#', '#', 'b', '@', '@', '#', '#', '@', 'b', '@', '@', '#', '#', 'b'},
  {'@', '@', '#', '@', 'b', 'b', '@', '@', '@', 'b', 'b', 'b', '@', '@', '@', 'b'},
  {'b', 'b', '@', 'b', '#', '#', 'b', 'b', 'b', 'b', '#', '#', 'b', 'b', 'b', '@'},
  {'#', '#', 'b', '@', '#', '#', 'b', '#', '#', 'b', '@', '#', '#', 'b', '@', '#'},
  {'@', '#', 'b', '@', '#', '@', 'b', '@', '#', '#', 'b', '@', '#', 'b', '@', '#'},
  {'@', '@', 'b', '@', '@', 'b', 'b', 'b', '@', '@', 'b', 'b', 'b', 'b', '@', '@'},
  {'b', 'b', '@', 'b', 'b', '@', '#', '#', 'b', 'b', '@', '#', '#', 'b', 'b', '@'}
};

const char bomb[16][16] PROGMEM =
{
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'r', 'r', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'w', 'w', 'w', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'w', 'w', 'w', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'w', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'w', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'}
};


const char explosion_center[16][16] PROGMEM =
{
  {'z', 'z', 'z', 'q', 'a', '1', '1', '1', '1', 'a', 'q', 'r', 'r', 'z', 'z', 'q'},
  {'q', 'z', 'q', 'a', '1', '1', '1', '1', '1', 'a', 'a', 'q', 'z', 'z', 'q', 'a'},
  {'q', 'a', 'a', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'q', 'q', 'q', 'a'},
  {'a', '1', 'a', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'a', 'q', 'a', 'a'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'a', 'a', 'a', '1'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'a', 'a', '1'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'a', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'a', '1', '1', '1', 'a', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
  {'q', 'a', '1', 'a', 'a', 'a', '1', '1', '1', '1', '1', '1', '1', 'a', 'a', '1'},
  {'q', 'q', 'a', 'a', 'q', 'a', '1', '1', '1', '1', '1', '1', '1', 'a', 'a', 'a'},
  {'z', 'z', 'q', 'q', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'q', 'a'},
  {'z', 'r', 'z', 'q', 'a', 'a', '1', '1', '1', '1', '1', '1', '1', 'a', 'q', 'q'},
};

const char explosion_right[16][16] PROGMEM =
{
  {'r', 'r', 'z', 'z', 'r', 'r', 'r', 'r', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'z', 'z', 'q', 'q', 'z', 'z', 'z', 'r', 'r', 'r', 'r', 'c', 'c', 'c', 'c', 'c'},
  {'q', 'q', 'q', 'q', 'q', 'q', 'z', 'z', 'z', 'z', 'r', 'r', 'r', 'r', 'c', 'c'},
  {'a', 'a', 'q', 'a', 'a', 'q', 'q', 'q', 'q', 'z', 'z', 'z', 'r', 'r', 'r', 'r'},
  {'1', 'a', 'a', '1', '1', 'a', 'a', 'a', 'q', 'q', 'q', 'z', 'z', 'r', 'r', 'c'},
  {'1', 'a', '1', '1', '1', '1', 'a', 'a', 'a', 'q', 'q', 'q', 'z', 'z', 'r', 'r'},
  {'1', '1', '1', '1', '1', '1', '1', 'a', 'a', 'a', 'q', 'q', 'q', 'z', 'r', 'c'},
  {'1', '1', '1', '1', 'a', '1', '1', '1', '1', 'a', 'a', 'q', 'z', 'r', 'r', 'r'},
  {'1', '1', '1', '1', '1', 'a', '1', '1', 'a', 'a', 'q', 'q', 'q', 'z', 'r', 'r'},
  {'1', '1', '1', '1', '1', '1', 'a', 'a', 'a', 'a', 'a', 'q', 'q', 'z', 'r', 'c'},
  {'1', '1', '1', '1', '1', '1', 'a', 'a', 'q', 'q', 'q', 'q', 'z', 'z', 'z', 'r'},
  {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'q', 'q', 'q', 'z', 'z', 'z', 'r', 'r', 'r'},
  {'a', 'q', 'q', 'q', 'q', 'q', 'q', 'q', 'z', 'z', 'z', 'z', 'r', 'r', 'r', 'c'},
  {'z', 'z', 'z', 'z', 'z', 'z', 'z', 'z', 'z', 'r', 'r', 'r', 'r', 'r', 'c', 'c'},
  {'r', 'r', 'z', 'r', 'r', 'z', 'r', 'r', 'r', 'r', 'r', 'c', 'c', 'c', 'c', 'c'},
  {'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'},
};

const char explosion_left[16][16] PROGMEM = {
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r'},
  {'c', 'c', 'c', 'c', 'c', 'r', 'r', 'r', 'r', 'r', 'z', 'r', 'r', 'z', 'r', 'r'},
  {'c', 'c', 'r', 'r', 'r', 'r', 'r', 'z', 'z', 'z', 'z', 'z', 'z', 'z', 'z', 'z'},
  {'c', 'r', 'r', 'r', 'z', 'z', 'z', 'z', 'q', 'q', 'q', 'q', 'q', 'q', 'q', 'a'},
  {'r', 'r', 'r', 'z', 'z', 'z', 'q', 'q', 'q', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
  {'r', 'z', 'z', 'z', 'q', 'q', 'q', 'q', 'a', 'a', '1', '1', '1', '1', '1', '1'},
  {'c', 'r', 'z', 'q', 'q', 'a', 'a', 'a', 'a', 'a', '1', '1', '1', '1', '1', '1'},
  {'r', 'r', 'z', 'q', 'q', 'q', 'a', 'a', '1', '1', 'a', '1', '1', '1', '1', '1'},
  {'r', 'r', 'r', 'z', 'q', 'a', 'a', '1', '1', '1', '1', 'a', '1', '1', '1', '1'},
  {'c', 'r', 'z', 'q', 'q', 'q', 'a', 'a', 'a', '1', '1', '1', '1', '1', '1', '1'},
  {'r', 'r', 'z', 'z', 'q', 'q', 'q', 'a', 'a', 'a', '1', '1', '1', '1', 'a', '1'},
  {'c', 'r', 'r', 'z', 'z', 'q', 'q', 'q', 'a', 'a', 'a', '1', '1', 'a', 'a', '1'},
  {'r', 'r', 'r', 'r', 'z', 'z', 'z', 'q', 'q', 'q', 'q', 'a', 'a', 'q', 'a', 'a'},
  {'c', 'c', 'r', 'r', 'r', 'r', 'z', 'z', 'z', 'z', 'q', 'q', 'q', 'q', 'q', 'q'},
  {'c', 'c', 'c', 'c', 'c', 'r', 'r', 'r', 'r', 'z', 'z', 'z', 'q', 'q', 'z', 'z'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'r', 'r', 'r', 'r', 'z', 'z', 'r', 'r'}
};

const char explosion_top[16][16] PROGMEM =
{
  {'c', 'c', 'c', 'c', 'r', 'r', 'c', 'r', 'r', 'c', 'r', 'r', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'r', 'r', 'z', 'r', 'r', 'r', 'r', 'z', 'r', 'r', 'c', 'c', 'c'},
  {'c', 'c', 'r', 'r', 'r', 'z', 'z', 'z', 'z', 'z', 'z', 'r', 'r', 'r', 'c', 'c'},
  {'c', 'c', 'r', 'r', 'z', 'z', 'q', 'q', 'q', 'q', 'z', 'z', 'r', 'r', 'c', 'c'},
  {'c', 'c', 'r', 'r', 'z', 'q', 'q', 'a', 'a', 'q', 'q', 'z', 'r', 'r', 'c', 'c'},
  {'c', 'r', 'r', 'r', 'z', 'q', 'a', 'a', 'a', 'a', 'q', 'z', 'r', 'r', 'r', 'c'},
  {'c', 'r', 'r', 'z', 'q', 'q', 'a', '1', '1', 'a', 'q', 'q', 'z', 'z', 'r', 'c'},
  {'c', 'r', 'z', 'z', 'q', 'q', 'a', '1', '1', 'a', 'a', 'q', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'q', 'a', 'a', '1', '1', 'a', 'a', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'a', 'a', 'a', '1', 'a', 'a', 'a', 'q', 'q', 'z', 'r', 'c'},
  {'r', 'z', 'z', 'q', 'a', '1', '1', 'a', 'a', '1', '1', 'a', 'q', 'z', 'z', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'z', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'z', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', 'a', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'a', 'a', '1', '1', '1', '1', 'a', 'a', 'q', 'z', 'r', 'r', 'r'},
};

const char explosion_bottom[16][16] PROGMEM =
{
  {'r', 'r', 'z', 'a', 'a', '1', '1', '1', '1', '1', '1', 'a', 'a', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'z', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'z', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'a', '1', '1', '1', '1', '1', '1', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'z', 'z', 'q', 'a', '1', '1', 'a', 'a', '1', '1', 'a', 'q', 'z', 'z', 'r'},
  {'c', 'r', 'z', 'q', 'q', 'a', 'a', 'a', '1', 'a', 'a', 'a', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'a', 'a', 'a', '1', '1', 'a', 'a', 'q', 'q', 'z', 'r', 'r'},
  {'r', 'r', 'z', 'q', 'q', 'a', 'a', '1', '1', 'a', 'q', 'q', 'z', 'z', 'r', 'c'},
  {'c', 'r', 'z', 'z', 'q', 'q', 'a', '1', '1', 'a', 'q', 'q', 'z', 'r', 'r', 'c'},
  {'c', 'r', 'r', 'r', 'z', 'q', 'a', 'a', 'a', 'a', 'q', 'z', 'r', 'r', 'r', 'c'},
  {'c', 'c', 'r', 'r', 'z', 'q', 'q', 'a', 'a', 'q', 'q', 'z', 'r', 'r', 'c', 'c'},
  {'c', 'c', 'r', 'r', 'z', 'z', 'q', 'q', 'q', 'q', 'z', 'z', 'r', 'r', 'c', 'c'},
  {'c', 'c', 'r', 'r', 'r', 'z', 'z', 'z', 'z', 'z', 'z', 'r', 'r', 'r', 'c', 'c'},
  {'c', 'c', 'c', 'r', 'r', 'z', 'r', 'r', 'r', 'r', 'z', 'r', 'r', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'r', 'r', 'c', 'r', 'r', 'c', 'r', 'r', 'c', 'c', 'c', 'c'},
};

const char bomber_front[24][16] PROGMEM =
{
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'b', 'r', 'r', 'r', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', '1', '1', 'b'},
  {'b', '1', '1', 'r', 'q', 'b', 'q', 'q', 'q', 'q', 'b', 'q', 'r', '1', '1', 'b'},
  {'b', '1', '1', 'r', 'q', 'b', 'q', 'q', 'q', 'q', 'b', 'q', 'r', '1', '1', 'b'},
  {'b', '1', '1', 'r', 'q', 'b', 'q', 'q', 'q', 'q', 'b', 'q', 'r', '1', '1', 'b'},
  {'b', '1', '1', 'r', 'q', 'b', 'q', 'q', 'q', 'q', 'b', 'q', 'r', '1', '1', 'b'},
  {'c', 'b', '1', '1', 'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', '1', '1', 'b', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'b', 'b', '1', 'b', '2', '2', '2', '2', '2', '2', 'b', '1', 'b', 'b', 'c'},
  {'b', 'r', 'r', 'r', 'b', '2', '2', '2', '2', '2', '2', 'b', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'b', 'b', 'b', 'q', 'q', 'b', 'b', 'b', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'b', '2', '2', '2', '2', '2', '2', 'b', 'r', 'r', 'r', 'b'},
  {'c', 'b', 'b', 'b', '2', '2', '2', '2', '2', '2', '2', '2', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'c', 'b', '1', '1', '1', '2', '2', '1', '1', '1', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', 'b', 'b', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'c'},
  {'c', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c'}
};

const char bomber_back[24][16] PROGMEM =
{
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'b', 'r', 'r', 'r', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'r', 'r', 'r', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', 'b', 'r', 'b', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', 'b', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'b', 'b', '1', 'b', '2', '2', '2', '2', '2', '2', 'b', '1', 'b', 'b', 'c'},
  {'b', 'r', 'r', '1', 'b', '2', '2', '2', '2', '2', '2', 'b', '1', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'b'},
  {'b', 'r', 'r', 'r', 'b', '2', '2', '2', '2', '2', '2', 'b', 'r', 'r', 'r', 'b'},
  {'c', 'b', 'b', 'b', '2', '2', '2', '2', '2', '2', '2', '2', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'c', 'b', '1', '1', '1', '2', '2', '1', '1', '1', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', 'b', 'b', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'c'},
  {'c', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c'}
};

const char bomber_left_side[24][16] PROGMEM =
{
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'b', 'r', 'r', 'r', 'b'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'b'},
  {'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'b', 'c'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c', 'c'},
  {'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', 'r', 'r', 'r', 'r', 'r', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', 'q', 'q', 'b', 'q', 'q', 'r', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', 'q', 'q', 'b', 'q', 'q', 'r', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', 'q', 'q', 'b', 'q', 'q', 'r', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'b', 'q', 'q', 'b', 'q', 'q', 'r', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'c', 'b', 'r', 'r', 'r', 'r', '1', '1', '1', '1', '1', '1', '1', 'b', 'c', 'c'},
  {'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'b', 'r', 'r', 'r', '1', '1', '1', 'b', '2', '2', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'b', 'r', 'r', 'r', '1', 'b', 'b', '2', '2', '2', '2', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'b', '2', '2', '2', '2', '2', '2', '2', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'b', '2', '2', '2', '2', '2', '2', 'b', 'c', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', '1', '1', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', '1', '1', 'b', 'b', 'b', 'b', 'b', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c'}
};

const char bomber_right_side[24][16] PROGMEM =
{


  {'c', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'b', 'r', 'r', 'r', 'b', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'},
  {'b', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'b', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b', 'c'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', '1', 'r', 'r', 'r', 'r', 'r', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', 'r', 'q', 'q', 'b', 'q', 'q', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', 'r', 'q', 'q', 'b', 'q', 'q', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', 'r', 'q', 'q', 'b', 'q', 'q', 'b'},
  {'c', 'b', '1', '1', '1', '1', '1', '1', '1', 'r', 'q', 'q', 'b', 'q', 'q', 'b'},
  {'c', 'c', 'b', '1', '1', '1', '1', '1', '1', '1', 'r', 'r', 'r', 'r', 'b', 'c'},
  {'c', 'c', 'c', 'b', 'b', '1', '1', '1', '1', '1', '1', '1', 'b', 'b', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'b', '2', '2', 'b', '1', '1', '1', 'r', 'r', 'r', 'b'},
  {'c', 'c', 'c', 'c', 'b', '2', '2', '2', '2', 'b', 'b', '1', 'r', 'r', 'r', 'b'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'c', 'c', 'b', '2', '2', '2', '2', '2', '2', '2', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'c', 'b', '2', '2', '2', '2', '2', '2', 'b', 'c', 'c', 'c'},
  {'c', 'c', 'c', 'c', 'b', 'b', 'b', 'b', '1', '1', 'b', 'b', 'c', 'c', 'c', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', '1', '1', 'b', 'b', 'b', 'b', 'b', 'c', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'b', 'b', 'b', 'b', 'b', 'r', 'r', 'r', 'r', 'r', 'b', 'b', 'b', 'b', 'c'},
  {'c', 'c', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'c', 'c'}
};
