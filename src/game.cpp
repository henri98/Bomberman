#include "game.h"

#define MAP_WIDTH         16 //18
#define MAP_HEIGHT        15 //16
#define SURROUND_WALLS    1
#define WALL_EMPTY_RATIO  3
#define BLOCKSIZE         16 //15

#define MASTER            1 //master sends the seed for the map, the masters player start location is 1,1

//@TODO!
#define BOMTIME           3 //seconds

char map_arr[MAP_HEIGHT][MAP_WIDTH];

MI0283QT9 lcd;
Player *player;
Player *opponent;

/*
 * This function initializes timer0 for the LCD screan, initializes the player and opponent,
 * initializes the screen draws the player and starts the game loop.
 */
void initinit()
{
  // set timer 0 for display libary, this is a part of the arduino init() function
  sei();
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
  // end part of arduino init() funciton

  // enable timer 0 overflow interrupt
  #if defined(TIMSK) && defined(TOIE0)
  sbi(TIMSK, TOIE0);
  #elif defined(TIMSK0) && defined(TOIE0)
  sbi(TIMSK0, TOIE0);
  #else
          #error  Timer 0 overflow interrupt not set correctly
  #endif

  //for debugging
  Serial.begin(250000);

  // init player and opponent
  player = (Player *)malloc(sizeof(Player));
  opponent = (Player *)malloc(sizeof(Player));

  //check if master
  if (MASTER)
    {
      init_player(player, 1, 1, 0, 3, blue);
      init_player(opponent, 12, 10, 0, 3, green);
    }
  else
    {
      init_player(player, 12, 10, 0, 3, blue);
      init_player(opponent, 1, 1, 0, 3, green);
    }

// init comunication and Nunchuck
  initIRCommLib();
  nunchuck_init();

  // start display and generate and load map

  init_display(lcd);
  generate_map();
  load_map(lcd);

  //draw player and opponent
  draw_player(player, lcd);
  draw_player(opponent, lcd);
  draw_lifes(player, lcd, 0);
  draw_lifes(opponent, lcd, 1);

  // start game loop
  gameloop(player, opponent, lcd);
}

/*
 * This function generate the map. The size of WALL_EMPTY_RATIO defines the percentage off
 * wall in the map.
 */
void generate_map()
{
  // @TODO: Make random ...
  srand(1);

  for (uint8_t y = 0; y < MAP_HEIGHT; y++)
    {
      for (uint8_t x = 0; x < MAP_WIDTH; x++)
        {
          // If surround_walls is enabled, and if it is currently filling an edge
          if ( SURROUND_WALLS != 0 && (x == 0 || y == 0 || x == (MAP_WIDTH - 1) || y == (MAP_HEIGHT - 1)) )
            {
              map_arr[y][x] = 'w';
            }
          else
            {
              map_arr[y][x] = (rand() % WALL_EMPTY_RATIO) == 0 ? 'n' : 'm';
            }
        }
    }

  uint8_t surrounded = SURROUND_WALLS != 0 ? 1 : 0;

  // Save a corner for the left-top player
  map_arr[surrounded][surrounded] = 'n';
  map_arr[surrounded + 1][surrounded] = 'n';
  map_arr[surrounded][surrounded + 1] = 'n';

  // Save a corner for the right-bot player
  map_arr[(MAP_HEIGHT - 1) - surrounded][(MAP_WIDTH - 1) - surrounded] = 'n';
  map_arr[((MAP_HEIGHT - 1) - surrounded) - 1][(MAP_WIDTH - 1) - surrounded] = 'n';
  map_arr[(MAP_HEIGHT - 1) - surrounded][((MAP_WIDTH - 1) - surrounded) - 1] = 'n';
}

/*
 * This function initializes a player.
 */
void init_player(Player *player, int x, int y, int points, int lifes, uint_least16_t color)
{
  player->location_x = x;
  player->location_y = y;
  player->points = points;
  player->lifes = lifes;
  player->color = color;
  player->bomblist[0].time_placed = 0;
  player->bomblist[0].explosion_removed = 1;
  player->bomblist[0].exploded = 1;
  player->bomblist[0].location_x = 100;
  player->bomblist[0].location_y = 100;
}

/*
 * This function starts the display
 */
void init_display(MI0283QT9 lcd)
{
  // init arduino library
  lcd.begin();
  lcd.led(0);
}

/*
 * Loads the map.
 */
void load_map(MI0283QT9 lcd)
{
  lcd.fillScreen(background);
  for (int y = 0; y < MAP_HEIGHT; y++)
    {
      for (int x = 0; x < MAP_WIDTH; x++)
        {
          if (map_arr[y][x] == 'w')
            {
              draw_object(lcd,x*BLOCKSIZE,y*BLOCKSIZE, wall);
            }
          else if (map_arr[y][x] == 'm')
            {
              //lcd.fillRect(x * BLOCKSIZE, y * BLOCKSIZE,BLOCKSIZE,BLOCKSIZE, RGB(134,232,0));
              draw_object(lcd,x*BLOCKSIZE,y*BLOCKSIZE, wall3);
            }
        }
    }
  lcd.led(100);
}

/*
 * This function draws a player on the right location.
 */
void draw_player(Player *player, MI0283QT9 lcd)
{
  draw_player_sprite(lcd, player->location_x*BLOCKSIZE, (player->location_y*BLOCKSIZE)-8, player->color, bomber_front);
}

/*
 * This function moves a player one position to the left.
 */
void move_left(Player *player,MI0283QT9 lcd, unsigned char opponent)
{
  draw_bomb(player, lcd);
  if (map_arr[player->location_y][player->location_x  - 1] == 'n')
    {
      if (map_arr[player->location_y-1][player->location_x] == 'w')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall);
        }
      else if (map_arr[player->location_y-1][player->location_x] == 'm')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall3);
        }
      else
        {
          lcd.fillRect(player->location_x*BLOCKSIZE, (player->location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
        }
      lcd.fillRect(player->location_x*BLOCKSIZE,player->location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
      player->location_x--;
      draw_player_sprite(lcd, player->location_x*BLOCKSIZE, (player->location_y*BLOCKSIZE)-8, player->color, bomber_left_side);
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      if (!opponent)
        {
          sendPlayerPos(&xPos, &yPos);
        }
    }
  check_if_player_in_bomb_explosion();
}

/*
 * This function moves a player one position to the right.
 */
void move_right(Player *player,MI0283QT9 lcd, unsigned char opponent)
{
  draw_bomb(player, lcd);
  if (map_arr[player->location_y][player->location_x  + 1] == 'n')
    {
      if (map_arr[player->location_y-1][player->location_x] == 'w')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall);
        }
      else if (map_arr[player->location_y-1][player->location_x] == 'm')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall3);
        }
      else
        {
          lcd.fillRect(player->location_x*BLOCKSIZE, (player->location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
        }
      lcd.fillRect(player->location_x*BLOCKSIZE,player->location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
      player->location_x++;
      draw_player_sprite(lcd, player->location_x*BLOCKSIZE, (player->location_y*BLOCKSIZE)-8, player->color, bomber_right_side);
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      if (!opponent)
        {
          sendPlayerPos(&xPos, &yPos);
        }
    }
  check_if_player_in_bomb_explosion();
}

/*
 * This function moves a player one position down.
 */
void move_down(Player *player,MI0283QT9 lcd,  unsigned char opponent)
{
  draw_bomb(player, lcd);
  if (map_arr[player->location_y + 1][player->location_x] == 'n')
    {
      if (map_arr[player->location_y-1][player->location_x] == 'w')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall);
        }
      else if (map_arr[player->location_y-1][player->location_x] == 'm')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall3);
        }
      else
        {
          lcd.fillRect(player->location_x*BLOCKSIZE, (player->location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
        }
      lcd.fillRect(player->location_x*BLOCKSIZE,player->location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
      player->location_y++;
      draw_player_sprite(lcd, player->location_x*BLOCKSIZE, (player->location_y*BLOCKSIZE)-8, player->color, bomber_front);
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      if (!opponent)
        {
          sendPlayerPos(&xPos, &yPos);
        }
    }
  check_if_player_in_bomb_explosion();
}

/*
 * This function moves a player one position up.
 */
void move_up(Player *player,MI0283QT9 lcd,  unsigned char opponent)
{
  draw_bomb(player, lcd);
  if (map_arr[player->location_y - 1][player->location_x  ] == 'n')
    {
      if (map_arr[player->location_y-1][player->location_x] == 'w')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall);
        }
      else if (map_arr[player->location_y-1][player->location_x] == 'm')
        {
          draw_object(lcd,player->location_x*BLOCKSIZE, (player->location_y - 1) *BLOCKSIZE, wall3);
        }
      else
        {
          lcd.fillRect(player->location_x*BLOCKSIZE, (player->location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
        }
      lcd.fillRect(player->location_x*BLOCKSIZE,player->location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
      player->location_y--;
      draw_player_sprite(lcd, player->location_x*BLOCKSIZE, (player->location_y*BLOCKSIZE)-8, player->color, bomber_back);
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      if (!opponent)
        {
          sendPlayerPos(&xPos, &yPos);
        }
    }
  check_if_player_in_bomb_explosion();
  draw_bomb(player, lcd);
}

/*
 * This function places a bomb.
 */
void place_bomb(Player *player)
{
  //check if bomb allready placed
  if ( player->bomblist[0].time_placed + 4000 <=  millis())
    {
      player->bomblist[0].location_x = player->location_x;
      player->bomblist[0].location_y = player->location_y;
      player->bomblist[0].exploded = 0;
      player->bomblist[0].explosion_removed = 0;
      player->bomblist[0].time_placed = millis();
    }
}

/*
 * This function draws a bomb.
 */
void draw_bomb(Player *player, MI0283QT9 lcd)
{
  if (player->bomblist[0].exploded != 1)
    {
      draw_object(lcd,player->bomblist[0].location_x*BLOCKSIZE,player->bomblist[0].location_y*BLOCKSIZE, bomb);
    }
}

void draw_lifes(Player *player, MI0283QT9 lcd, int opponent)
{
  int height = 0;
  if (opponent)
    {
      height += 197;
    }
  lcd.setTextColor(player->color, background);
  lcd.setCursor(261, height + 2);
  if (opponent)
    {
      lcd.println("Opposer");
    }
  else
    {
      lcd.println("You");
    }
  for (unsigned int q = 0; q < 3; q++)
    {
      if (q< player->lifes)
        {
          //  lcd.fillCircle(268 + (q*18), height + 20, 8, RGB(0,0,0));
          draw_object(lcd, 260 + (q*18), height + 12, life);
        }
      else
        {
          lcd.fillRect(260 + (q*18), height + 12, BLOCKSIZE,BLOCKSIZE, background);
        }

    }
  lcd.setCursor(261, height + 31);
  lcd.println(player->points);
}

void check_if_player_in_bomb_explosion()
{
  if ((player->location_x == player->bomblist[0].location_x && player->location_y == player->bomblist[0].location_y && player->bomblist[0].exploded == 1 && player->bomblist[0].explosion_removed != 1) ||
      (player->location_x == player->bomblist[0].location_x - 1 && player->location_y == player->bomblist[0].location_y && player->bomblist[0].exploded == 1 && player->bomblist[0].explosion_removed != 1) ||
      (player->location_x == player->bomblist[0].location_x + 1 && player->location_y == player->bomblist[0].location_y && player->bomblist[0].exploded == 1 && player->bomblist[0].explosion_removed != 1) ||
      (player->location_x == player->bomblist[0].location_x && player->location_y == player->bomblist[0].location_y - 1 && player->bomblist[0].exploded == 1 && player->bomblist[0].explosion_removed != 1)||
      (player->location_x == player->bomblist[0].location_x && player->location_y == player->bomblist[0].location_y + 1 && player->bomblist[0].exploded == 1 && player->bomblist[0].explosion_removed != 1)
      )
    {
      if (player->lifes != 0)
        {
          player->lifes--;
        }
      draw_lifes(player, lcd, 0);
      //@TODO! send lifes
    }
}

/*
 * This is the game loop.
 */
void gameloop(Player *player, Player *opponent, MI0283QT9 lcd)
{
  while (1)
    {
      // struct buf *buffer;
      struct buf *buffer = (buf *)malloc(sizeof(struct buf));
      nunchuck_get_data(buffer);
      delay(75);

      // // check if opponent moved
      // if ((opponent->location_x) > (getOpponentPosX()- '0'))
      //   {
      //     while ((opponent->location_x) != (getOpponentPosX()- '0'))
      //       {
      //         move_left(opponent, lcd, 1);
      //       }
      //   }
      //
      // if ((opponent->location_x) < (getOpponentPosX()- '0'))
      //   {
      //     while ((opponent->location_x) != (getOpponentPosX()- '0'))
      //       {
      //         move_right(opponent, lcd, 1);
      //       }
      //   }
      //
      // if ((opponent->location_y) > (getOpponentPosY()- '0'))
      //   {
      //     while ((opponent->location_y) != (getOpponentPosY()- '0'))
      //       {
      //         move_up(opponent, lcd, 1);
      //       }
      //   }
      //
      // if ((opponent->location_y) < (getOpponentPosY()- '0'))
      //   {
      //     while ((opponent->location_y) != (getOpponentPosY()- '0'))
      //       {
      //         move_down(opponent, lcd, 1);
      //       }
      //   }

      //check if bom has to explode
      if ( player->bomblist[0].time_placed + 3000 <=  millis() && player->bomblist[0].exploded != 1)
        {
          //bom has to explode!
          player->bomblist[0].exploded = 1;
          draw_object(lcd, player->bomblist[0].location_x * BLOCKSIZE, player->bomblist[0].location_y * BLOCKSIZE, explosion_center);
          //check if wall
          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x - 1] != 'w')
            {
              draw_object(lcd, (player->bomblist[0].location_x- 1) * BLOCKSIZE, player->bomblist[0].location_y * BLOCKSIZE, explosion_left);
            }

          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x + 1] != 'w')
            {
              draw_object(lcd, (player->bomblist[0].location_x + 1) * BLOCKSIZE, player->bomblist[0].location_y * BLOCKSIZE, explosion_right);
            }

          if (map_arr[player->bomblist[0].location_y - 1][player->bomblist[0].location_x] != 'w')
            {
              draw_object(lcd, player->bomblist[0].location_x * BLOCKSIZE, (player->bomblist[0].location_y-1) * BLOCKSIZE, explosion_top);
            }

          if (map_arr[player->bomblist[0].location_y + 1 ][player->bomblist[0].location_x] != 'w')
            {
              draw_object(lcd, player->bomblist[0].location_x * BLOCKSIZE, (player->bomblist[0].location_y+1) * BLOCKSIZE, explosion_bottom);
            }
          //check if opjects can explosive
          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x - 1] == 'm')
            {
              map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x - 1] = 'n';
              player->points += 25;
            }
          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x + 1] == 'm')
            {
              map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x + 1] = 'n';
              player->points += 26;
            }
          if (map_arr[player->bomblist[0].location_y - 1][player->bomblist[0].location_x] == 'm')
            {
              map_arr[player->bomblist[0].location_y - 1][player->bomblist[0].location_x] = 'n';
              player->points += 24;
            }
          if (map_arr[player->bomblist[0].location_y + 1 ][player->bomblist[0].location_x] == 'm')
            {
              map_arr[player->bomblist[0].location_y + 1 ][player->bomblist[0].location_x] = 'n';
              player->points += 23;
            }
          draw_lifes(player, lcd, 0);
          //check if there is a player.
          check_if_player_in_bomb_explosion();
        }
      //check if bom is exploded and animation have to be removed
      if ( player->bomblist[0].time_placed + 4000 <=  millis() && player->bomblist[0].explosion_removed != 1)
        {
          lcd.fillRect(player->bomblist[0].location_x*BLOCKSIZE,player->bomblist[0].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x - 1] != 'w')
            {
              lcd.fillRect((player->bomblist[0].location_x - 1)*BLOCKSIZE,player->bomblist[0].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }

          if (map_arr[player->bomblist[0].location_y][player->bomblist[0].location_x + 1] != 'w')
            {
              lcd.fillRect((player->bomblist[0].location_x + 1)*BLOCKSIZE,player->bomblist[0].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }

          if (map_arr[player->bomblist[0].location_y - 1][player->bomblist[0].location_x] != 'w')
            {
              lcd.fillRect(player->bomblist[0].location_x*BLOCKSIZE,(player->bomblist[0].location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }

          if (map_arr[player->bomblist[0].location_y + 1 ][player->bomblist[0].location_x] != 'w')
            {
              lcd.fillRect(player->bomblist[0].location_x*BLOCKSIZE,(player->bomblist[0].location_y + 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }
          player->bomblist[0].explosion_removed = 1;
          draw_player(player, lcd);
          draw_player(opponent, lcd);
        }

      //left
      if (buffer->xJoystick >= 34 && buffer->xJoystick <= 50 && buffer->yJoystick >= 98 && buffer->yJoystick <= 165)
        {
          move_left(player, lcd, 0);
        }
      /* Move Right */
      if (buffer->xJoystick >= 215 && buffer->xJoystick <= 235 && buffer->yJoystick >= 128 && buffer->yJoystick <= 175)
        {
          move_right(player, lcd, 0);
        }
      /* Move Up */
      if (buffer->xJoystick >= 90 && buffer->xJoystick <= 170 && buffer->yJoystick >= 215 && buffer->yJoystick <= 225)
        {
          move_up(player, lcd, 0);
        }
      /* Move Down */
      if (buffer->xJoystick >= 95 && buffer->xJoystick <= 155 && buffer->yJoystick >= 35 && buffer->yJoystick <= 52)
        {
          move_down(player, lcd, 0);
        }

      if (buffer->zButton == 1)
        {
          place_bomb(player);
        }
      free(buffer);
    }
}

ISR(TIMER2_OVF_vect)
{


}
