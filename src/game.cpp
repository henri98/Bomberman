#include "game.h"

uint_least16_t background = RGB(103, 98, 96);
volatile unsigned long overflow_counter = 0;

// char map_arr[14][12] = {
//   {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},   // 0
//   {'w', 'w', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 1
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 2
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 3
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 4
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 5
//   {'w', 'n', 'n', 'w', 'w', 'w', 'n', 'n', 'n', 'n', 'n', 'w'},   // 6
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 7
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 8
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 9
//   {'w', 'n', 'n', 'n', 'n', 'w', 'w', 'w', 'n', 'n', 'n', 'w'},   //10
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},     //11
//   {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 12
//   {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'}    // 13
// };

#define MAP_WIDTH         14
#define MAP_HEIGHT        12
#define SURROUND_WALLS    1
#define WALL_EMPTY_RATIO  3

char map_arr[MAP_WIDTH][MAP_HEIGHT];

void generate_map()
{
  srand(TCNT2);

  for (uint8_t y = 0; y < MAP_HEIGHT; y++)
  {
    for (uint8_t x = 0; x < MAP_WIDTH; x++)
    {
      // If surround_walls is enabled, and if it is currently filling an edge
      if ( SURROUND_WALLS != 0 && (x == 0 || y == 0 || x == (MAP_WIDTH - 1) || y == (MAP_HEIGHT - 1)) )
      {
        map_arr[x][y] = 'w';
      }
      else
      {
        map_arr[x][y] = (rand() % WALL_EMPTY_RATIO) == 0 ? 'n' : 'w';
      }
    }
  }

  uint8_t surrounded = SURROUND_WALLS != 0 ? 1 : 0;

  // Save a corner for the left-top player
  map_arr[surrounded][surrounded] = 'n';
  map_arr[surrounded + 1][surrounded] = 'n';
  map_arr[surrounded][surrounded + 1] = 'n';

  // Save a corner for the right-bot player
  map_arr[(MAP_WIDTH - 1) - surrounded][(MAP_HEIGHT - 1) - surrounded] = 'n';
  map_arr[((MAP_WIDTH - 1) - surrounded) - 1][(MAP_HEIGHT - 1) - surrounded] = 'n';
  map_arr[(MAP_WIDTH - 1) - surrounded][((MAP_HEIGHT - 1) - surrounded) - 1] = 'n';
}

void init_player(Player *player, int x, int y, int points, int lives, uint_least16_t color)
{
  player->location_x = x;
  player->location_y = y;
  player->points = points;
  player->lives = lives;
  player->color = color;
}

void init_display(MI0283QT9 lcd)
{
  // init arduino library
  init();
  lcd.begin();
//  lcd.begin(SPI_CLOCK_DIV128 );
  lcd.led(100);
}

void load_map(MI0283QT9 lcd)
{
  lcd.fillScreen(background);
  for (int y = 0; y < MAP_HEIGHT; y++)
  {
    for (int x = 0; x < MAP_WIDTH; x++)
    {
      if (map_arr[x][y] == 'w')
      {
        lcd.fillRect(x * 20, y * 20, 20, 4, RGB(139,0,0));
        lcd.drawLine(x * 20 + 19, y*20,  x * 20 + 19, y*20 +4, RGB(0,0,0));
        lcd.fillRect(x * 20, y * 20 +4, 20, 1, RGB(47,79,79));
        lcd.fillRect(x * 20, y * 20 +5, 20, 4, RGB(139,0,0));
        lcd.drawLine(x * 20 + 9, y*20 +5,  x * 20 + 9, y*20 +9, RGB(0,0,0));
        lcd.fillRect(x * 20, y * 20 +9, 20, 1, RGB(47,79,79));
        lcd.fillRect(x * 20, y * 20 +10, 20, 4, RGB(139,0,0));
        lcd.drawLine(x * 20 + 19, y*20 + 10,  x * 20 + 19, y*20 +14, RGB(0,0,0));
        lcd.fillRect(x * 20, y * 20 +14, 20, 1, RGB(47,79,79));
        lcd.fillRect(x * 20, y * 20 +15, 20, 4, RGB(139,0,0));
        lcd.drawLine(x * 20 + 9, y*20 + 15,  x * 20 + 9, y*20 +19, RGB(0,0,0));
        lcd.fillRect(x * 20, y * 20 +19, 20, 1, RGB(47,79,79));
      }
    }
  }
}

void draw_player(Player *player,MI0283QT9 lcd)
{
  lcd.fillRect(player->location_x*20,player->location_y*20,20,20,player->color);
}

void move_left(Player *player,MI0283QT9 lcd)
{
  if (map_arr[player->location_y][player->location_x  - 1] != 'w')
  {
    for (size_t i = 0; i < 20; i++)
    {
      //add line
      lcd.drawLine(player->location_x * 20 - i, player->location_y * 20, player->location_x * 20 - i, player->location_y * 20 + 19, player->color);
      //remove line
      lcd.drawLine(player->location_x * 20 + 19 - i, player->location_y * 20, player->location_x * 20 + 19 - i, player->location_y * 20 + 19, background);
      wait(100);
    }
    lcd.drawLine(player->location_x * 20 - 20, player->location_y * 20, player->location_x * 20 - 20, player->location_y * 20 + 19, player->color);
    player->location_x--;
  }
  draw_bomb(player, lcd);

}

void move_right(Player *player,MI0283QT9 lcd)
{
  if (map_arr[player->location_y][player->location_x  + 1] != 'w')
  {
    for (size_t i = 0; i < 20; i++)
    {
      lcd.drawLine(player->location_x * 20 + 19 + i, player->location_y * 20, player->location_x * 20 + 19 + i, player->location_y * 20 + 19, player->color);
      lcd.drawLine(player->location_x * 20 + i, player->location_y * 20, player->location_x * 20 + i, player->location_y * 20 + 19, background);
      wait(100);
    }
    lcd.drawLine(player->location_x * 20 + 19 + 20, player->location_y * 20, player->location_x * 20 + 19 + 20, player->location_y * 20 + 19, player->color);
    player->location_x++;
  }
  draw_bomb(player, lcd);

}

void move_down(Player *player,MI0283QT9 lcd)
{
  if (map_arr[player->location_y + 1][player->location_x] != 'w')
  {
    for (size_t i = 0; i < 20; i++)
    {
      lcd.drawLine(player->location_x * 20, player->location_y * 20 + 19 + i, player->location_x * 20 + 19, player->location_y * 20 + 19 + i, player->color);
      lcd.drawLine(player->location_x * 20, player->location_y * 20 + i, player->location_x * 20 + 19, player->location_y * 20 + i, background);
      wait(100);
    }
    lcd.drawLine(player->location_x * 20, player->location_y * 20 + 19 + 20, player->location_x * 20 + 19, player->location_y * 20 + 19 + 20, player->color);
    player->location_y++;
  }
  draw_bomb(player, lcd);

}

void move_up(Player *player,MI0283QT9 lcd)
{
  if (map_arr[player->location_y - 1][player->location_x  ] != 'w')
  {
    for (size_t i = 0; i < 20; i++)
    {
      lcd.drawLine(player->location_x * 20, player->location_y * 20 - i, player->location_x * 20 + 19, player->location_y * 20 - i, player->color);
      lcd.drawLine(player->location_x * 20, player->location_y * 20 + 19 - i, player->location_x * 20 + 19, player->location_y * 20 + 19 - i,  background);
      wait(100);
    }
    lcd.drawLine(player->location_x * 20, player->location_y * 20 - 20, player->location_x * 20 + 19, player->location_y * 20 - 20, player->color);
    player->location_y--;
  }
  draw_bomb(player, lcd);
}

void place_bomb(Player *player)
{
  //check if bomb allready placed
  if (player->bomblist[0].time_placed < overflow_counter + 100000000)
  {
    player->bomblist[0].location_x = player->location_x;
    player->bomblist[0].location_y = player->location_y;
    player->bomblist[0].time_placed = overflow_counter;
  }
  //bomb->player = player;
  //lcd.fillCircle(bomb->location_x*20+10, bomb->location_y*20+10, 9.5, RGB(0,0,0));
}

void draw_bomb(Player *player, MI0283QT9 lcd)
{
  lcd.fillCircle(player->bomblist[0].location_x * 20+9, player->bomblist[0].location_y * 20+ 9, 9, RGB(0,0,0));
}

void gameloop(Player *player, MI0283QT9 lcd)
{
  while (1)
  {
    // struct buf *buffer;
    struct buf *buffer = (buf *)malloc(sizeof(struct buf));
    nunchuck_get_data(buffer);
    wait(12000);
    //left
    if (buffer->xJoystick >= 34 && buffer->xJoystick <= 50 && buffer->yJoystick >= 98 && buffer->yJoystick <= 165)
    {
      move_left(player, lcd);
    }
    /* Move Right */
    if (buffer->xJoystick >= 215 && buffer->xJoystick <= 235 && buffer->yJoystick >= 128 && buffer->yJoystick <= 175)
    {
      move_right(player, lcd);
    }
    /* Move Up */
    if (buffer->xJoystick >= 90 && buffer->xJoystick <= 170 && buffer->yJoystick >= 215 && buffer->yJoystick <= 225)
    {
      move_up(player, lcd);
    }
    /* Move Down */
    if (buffer->xJoystick >= 95 && buffer->xJoystick <= 155 && buffer->yJoystick >= 35 && buffer->yJoystick <= 52)
    {
      move_down(player, lcd);
    }

    if (buffer->zButton == 1)
    {
      place_bomb(player);
    }
    free(buffer);
  }
}

void wait(unsigned long delaytime)
{
  unsigned long currenttime = overflow_counter;
  while (overflow_counter  <= currenttime + delaytime)
  {
    //delay
  }
}

ISR(TIMER2_OVF_vect)
{
  overflow_counter++;
}
