#include "game.h"

static uint_least16_t background = RGB(103, 98, 96);

#define MAP_WIDTH         14
#define MAP_HEIGHT        12
#define SURROUND_WALLS    1
#define WALL_EMPTY_RATIO  2
#define BOMTIME           3 //seconds

char map_arr[MAP_HEIGHT][MAP_WIDTH];

MI0283QT9 lcd;
Player *player1;
Player *oponent;

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

  // init player and oponent
  player1 = (Player *)malloc(sizeof(Player));
  oponent = (Player *)malloc(sizeof(Player));
  player1->bomblist[0].time_placed = 0;
  oponent->bomblist[0].time_placed = 0;
  player1->bomblist[0].location_x = 100;
  player1->bomblist[0].location_y = 100;
  oponent->bomblist[0].location_x = 100;
  oponent->bomblist[0].location_y = 100;
  init_player(player1, 1, 1, 0, 3, RGB(150,100,250));
  init_player(oponent, 12, 10, 0, 3, RGB(44, 76, 23));


  // init comunication and Nunchuck
  initIRCommLib();
  nunchuck_init();

  // start display and generate and load map

  init_display(lcd);
  generate_map();
  load_map(lcd);

  //draw player and oponent
  draw_player(player1, lcd);
  draw_player(oponent, lcd);

  // start game loop
  gameloop(player1, oponent, lcd);
}


void generate_map()
{
  // @TODO: Make random ...
  srand(analogRead(2));

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
              map_arr[y][x] = (rand() % WALL_EMPTY_RATIO) == 0 ? 'n' : 'w';
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
  lcd.begin();
  lcd.led(100);
}

void load_map(MI0283QT9 lcd)
{
  lcd.fillScreen(background);
  for (int y = 0; y < MAP_HEIGHT; y++)
    {
      for (int x = 0; x < MAP_WIDTH; x++)
        {
          if (map_arr[y][x] == 'w')
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
        }
      lcd.drawLine(player->location_x * 20 - 20, player->location_y * 20, player->location_x * 20 - 20, player->location_y * 20 + 19, player->color);
      player->location_x--;
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      sendPlayerPos(&xPos, &yPos);
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

        }
      lcd.drawLine(player->location_x * 20 + 19 + 20, player->location_y * 20, player->location_x * 20 + 19 + 20, player->location_y * 20 + 19, player->color);
      player->location_x++;
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      sendPlayerPos(&xPos, &yPos);
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
        }
      lcd.drawLine(player->location_x * 20, player->location_y * 20 + 19 + 20, player->location_x * 20 + 19, player->location_y * 20 + 19 + 20, player->color);
      player->location_y++;
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      sendPlayerPos(&xPos, &yPos);
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
        }
      lcd.drawLine(player->location_x * 20, player->location_y * 20 - 20, player->location_x * 20 + 19, player->location_y * 20 - 20, player->color);
      player->location_y--;
      unsigned char xPos = player->location_x + '0';
      unsigned char yPos = player->location_y + '0';
      sendPlayerPos(&xPos, &yPos);
    }

  draw_bomb(player, lcd);
}

void place_bomb(Player *player)
{
  //check if bomb allready placed
  if ( player->bomblist[0].time_placed + 3000 <=  millis())
    {
      player->bomblist[0].location_x = player->location_x;
      player->bomblist[0].location_y = player->location_y;
      player->bomblist[0].exploded = 0;
      player->bomblist[0].time_placed = millis();
    }
}

void draw_bomb(Player *player, MI0283QT9 lcd)
{
  lcd.fillCircle(player->bomblist[0].location_x * 20+9, player->bomblist[0].location_y * 20+ 9, 9, RGB(0,0,0));
}

void gameloop(Player *player, Player *opponent, MI0283QT9 lcd)
{
  while (1)
    {
      // struct buf *buffer;
      struct buf *buffer = (buf *)malloc(sizeof(struct buf));
      nunchuck_get_data(buffer);
      delay(100);

      // Get the received opponent's location
      opponent->location_x = getOpponentPosX();
      opponent->location_y = getOpponentPosY();

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

ISR(TIMER2_OVF_vect)
{
  if ( player1->bomblist[0].time_placed + 3000 <=  millis() && player1->bomblist[0].exploded != 1)
    {
      //bom has to explode!
      player1->bomblist[0].exploded = 1;
      Serial.println("Boem");
    }
}
