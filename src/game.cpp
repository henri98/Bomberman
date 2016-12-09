#include "game.h"

uint_least16_t background = RGB(103, 98, 96);
volatile unsigned long overflow_counter = 0;

char map_arr[12][14] = {
  {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},   // 0
  {'w', 'w', 'n', 'w', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'w', 'w'},   // 1
  {'w', 'n', 'n', 'n', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 2
  {'w', 'w', 'w', 'w', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 3
  {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 4
  {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w', 'n', 'n', 'n', 'w'},   // 5
  {'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 6
  {'w', 'n', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 7
  {'w', 'n', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 8
  {'w', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 9
  {'w', 'w', 'n', 'n', 'n', 'w', 'n', 'n', 'n', 'n', 'n', 'n', 'w'},   // 10
  {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'}    // 11
};

void generate_map()
{
  // generate map....
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
  for (size_t i = 0; i < 14; i++)
    {
      for (size_t j = 0; j < 12; j++)
        {
          if (map_arr[j][i] == 'w')
            {
              lcd.fillRect(i*20,j*20,20,20,RGB(45,45, 45));
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
          wait(10);
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
          wait(10);
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
          wait(10);
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
          wait(10);
        }
      lcd.drawLine(player->location_x * 20, player->location_y * 20 - 20, player->location_x * 20 + 19, player->location_y * 20 - 20, player->color);
      player->location_y--;
    }
  draw_bomb(player, lcd);
}

void place_bomb(Player *player)
{
  player->bomblist[0].location_x = player->location_x;
  player->bomblist[0].location_y = player->location_y;
  player->bomblist[0].time_placed = overflow_counter;

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

      wait(10);
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
    }
}

ISR(TIMER2_OVF_vect)
{
  overflow_counter++;
}
