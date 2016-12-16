#include <stdio.h>
#include <stdlib.h>
#include <MI0283QT9.h>

#define WIDTH        17 //18
#define HEIGHT       16 //16

static uint_least16_t red = RGB(255, 0, 0);
static uint_least16_t green = RGB(0, 255, 0);
static uint_least16_t blue = RGB(0, 0, 255);
static uint_least16_t white = RGB(255, 255, 255);
static uint_least16_t black = RGB(0, 0, 0);
static uint_least16_t background = RGB(103, 98, 96);

void draw_object(MI0283QT9 lcd, int x, int y, char sprite[16][16]);

static char life[16][16] =
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
