#include "game.h"

#define MAP_WIDTH         16 //18
#define MAP_HEIGHT        15 //16
#define SURROUND_WALLS    1
#define WALL_EMPTY_RATIO  2
#define BLOCKSIZE         16 //15

#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[0] ))

unsigned int master =     0; //master sends the seed for the map, the masters player start location is 1,1
unsigned char seed;

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

  // enable timer 0 overflow interrupt
  #if defined(TIMSK) && defined(TOIE0)
  sbi(TIMSK, TOIE0);
  #elif defined(TIMSK0) && defined(TOIE0)
  sbi(TIMSK0, TOIE0);
  #else
          #error  Timer 0 overflow interrupt not set correctly
  #endif
  // end part of arduino init() funciton

  //for debugging
  Serial.begin(250000);

  // init adc
  init_ADC();

  // init player and opponent
  player = (Player *)malloc(sizeof(Player));
  opponent = (Player *)malloc(sizeof(Player));

  // init comunication and Nunchuck
  initIRCommLib();
  nunchuck_init();

  // start display and start menu
  init_display(lcd);

  menu();
}

void start_game()
{
  if (!received_seed)
    {
      // make master
      master = 1;
      // get seed
      seed = (unsigned char) (micros()/4);
      // send seed
      sendSeed(seed);
    }

  //check if master
  if (master)
    {
      init_player(player, 1, 1, 0, 3, blue);
      init_player(opponent, MAP_WIDTH-2, MAP_HEIGHT-2, 0, 3, green);
    }
  else
    {
      init_player(player, MAP_WIDTH-2, MAP_HEIGHT-2, 0, 3, blue);
      init_player(opponent, 1, 1, 0, 3, green);
    }

  //generate and load map
  generate_map(seed);
  load_map(lcd);

  //draw player and opponent
  draw_player(player, lcd);
  draw_player(opponent, lcd);
  draw_lifes(player, lcd, 0);
  draw_lifes(opponent, lcd, 1);

  // start game loop
  gameloop(player, opponent, lcd);
}

void stop_game()
{
  lcd.fillScreen(black);
  if (player->lifes == opponent->lifes)
    {
      lcd.println("Gelijkspel");
      save_score(player->points);
    }
  else if (player->lifes > opponent->lifes)
    {
      lcd.drawText(90, 80, "You Win!", green, RGB(0,0,0), 2);
      lcd.setCursor(110,140);
      lcd.setTextColor(white, black);
      lcd.print("Score:");
      lcd.setCursor(170,140);
      lcd.print(player->points);

      save_score(player->points);
    }
  else if (player->lifes < opponent->lifes)
    {
      lcd.drawText(70, 80, "You Lose :(", red, RGB(0,0,0), 2);
      lcd.setCursor(110,140);
      lcd.setTextColor(white, black);
      lcd.print("Score:");
      lcd.setCursor(170,140);
      lcd.print(player->points);

      save_score(opponent->points);
    }
  delay (5000);
}

void menu()
{
  lcd.fillScreen(black);
  lcd.led(100);
  lcd.drawRect(19, 19, 280, 150, red);
  lcd.drawRect(20, 20, 280, 150, red);
  lcd.drawRect(21, 21, 280, 150, red);
  lcd.drawRect(22, 22, 280, 150, red);
  lcd.drawText(51, 80, "BOMBERMAN", yellow, RGB(0,0,0), 3);

  int menu[2] = {0, 1};
  uint8_t selected_menu_item = 0;
  uint8_t previous_selected_menu_item = 0;
  uint8_t previous_selected_menu_item_removed = 1;
  struct buf *buffer = (buf *)malloc(sizeof(struct buf));
  do
    {
      for (size_t i = 0; i < ARRAY_SIZE(menu); i++)
        {
          if (menu[i] == 0)
            {
              lcd.drawText(140, 194 + (i * 10), "START", white, black, 1);
            }
          if (menu[i] == 1)
            {
              lcd.drawText(140, 194 + (i * 10), "HIGHSCORES", white, black, 1);

            }
          if (selected_menu_item == i)
            {
              if (previous_selected_menu_item_removed == 0)
                {
                  lcd.fillRect(120, 194 + (previous_selected_menu_item * 10) - 4, 16, 16, black);
                  previous_selected_menu_item_removed = 1;
                }
              draw_object(lcd, 120,  194 + (i * 10) - 4, play_button);
            }
        }
      nunchuck_get_data(buffer);
      delay(5);
      //check if seed received from master!
      if (received_seed)
        {
          seed = up_to_date_seed;
          break;
        }
      if (check_if_joystick_up(buffer))
        {
          if (selected_menu_item != 0)
            {
              previous_selected_menu_item = selected_menu_item;
              previous_selected_menu_item_removed = 0;
              selected_menu_item--;
            }
        }
      if (check_if_joystick_down(buffer))
        {
          if (selected_menu_item < ARRAY_SIZE(menu)-1)
            {
              previous_selected_menu_item = selected_menu_item;
              previous_selected_menu_item_removed = 0;
              selected_menu_item++;
            }
        }
      lcd.led(get_ADC());
    }
  while (buffer->zButton != 1);
  free(buffer);
  if (selected_menu_item == 0)
    {
      start_game();
    }
  if (selected_menu_item == 1)
    {
      highscores();
    }
}



int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return 1;
     else return -1;
}

void highscores()
{
  lcd.fillScreen(black);
  struct buf *buffer = (buf *)malloc(sizeof(struct buf));
  int margin = 0;
  const char *names[5];
  names[0] = "Platinum";
  names[1] = "Gold";
  names[2] = "Silver";
  names[3] = "Bronze";
  names[4] = "Copper";
  lcd.drawText(80, 25, "HIGHSCORES", white, RGB(0,0,0), 2);
  int highscores[5];



  for (size_t i = 0; i < 5; i++)
    {
     highscores[i] = EEPROM_read_uint16_t(0 + (i * 2));
    }



    qsort(highscores, 5, sizeof(int), compare );


  for (size_t i = 0; i < 5; i++)
    {
      lcd.drawText(80, 80 + margin, names[i], white, RGB(0,0,0), 1);
  //    lcd.drawText(200, 80 + margin, (char) test[i], white, RGB(0,0,0), 1);
      lcd.setCursor(200,80 + margin);


      lcd.setTextColor(white, black);

      lcd.println(highscores[i]);




      lcd.drawRect(60, 75 + margin, 200, 20, red);
      lcd.drawRect(59, 74 + margin, 200, 20, red);

      margin = margin + 30;




    }
  do
    {
      nunchuck_get_data(buffer);
      delay(5);
  	  lcd.led(get_ADC());    }
  while (buffer->zButton != 1);
  free(buffer);
}

/*
 * This function generate the map. The size of WALL_EMPTY_RATIO defines the percentage off
 * wall in the map.
 */
void generate_map(unsigned long seed)
{
  srand(seed);
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

  for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
    {
      player->bomblist[i].explosion_removed = 1;
      player->bomblist[i].exploded = 1;
    }
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
 * This function places a bomb.
 */
void place_bomb(Player *player)
{
  unsigned char locX = player->location_x;
  unsigned char locY = player->location_y;
  uint8_t canPlaceBomb = 1;

  for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
    {
      if (player->bomblist[i].exploded == 0 && player->bomblist[i].location_x == locX && player->bomblist[i].location_y == locY)
        {
          canPlaceBomb = 0;
          break;
        }
    }

  if (canPlaceBomb == 1)
    for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
      {
        if (player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed == 1)
          {
            player->bomblist[i].location_x = player->location_x;
            player->bomblist[i].location_y = player->location_y;
            player->bomblist[i].exploded = 0;
            player->bomblist[i].isExploding = 0;
            player->bomblist[i].explosion_removed = 0;
            player->bomblist[i].time_placed = millis();
            sendBombPlaced(player->bomblist[i].location_x, player->bomblist[i].location_y, i);
            break;
          }
      }
}

/*
 * This function draws a bomb.
 */
void draw_bomb(Player *player, MI0283QT9 lcd)
{
  for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
    {
      if (player->bomblist[i].exploded != 1 && player->bomblist[i].isExploding != 1)
        {
          draw_object(lcd,player->bomblist[i].location_x*BLOCKSIZE,player->bomblist[i].location_y*BLOCKSIZE, bomb);
        }
    }

  for (uint8_t i = 0; i < ARRAY_SIZE(opponent->bomblist); i++)
    {
      if (opponent->bomblist[i].exploded != 1 && opponent->bomblist[i].isExploding != 1)
        {
          draw_object(lcd,opponent->bomblist[i].location_x*BLOCKSIZE,opponent->bomblist[i].location_y*BLOCKSIZE, bomb);
        }
    }
}

void get_opponent_bombs()
{
  for (uint8_t i = 0; i < ARRAY_SIZE(receivedBombs); i++)
    {
      if (receivedBombs[i].is_used == 0)
        {
          opponent->bomblist[i].location_x = receivedBombs[i].location_x;
          opponent->bomblist[i].location_y = receivedBombs[i].location_y;
          opponent->bomblist[i].exploded = 0;
          opponent->bomblist[i].isExploding = 0;
          opponent->bomblist[i].explosion_removed = 0;
          opponent->bomblist[i].time_placed = millis();
          receivedBombs[i].is_used = 1;
        }
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
      lcd.println("Enemy");
    }
  else
    {
      lcd.println("You");
    }
  for (unsigned int q = 0; q < 3; q++)
    {
      if (q < player->lifes)
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
  for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
    {
      if ((player->location_x == player->bomblist[i].location_x && player->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (player->location_x == player->bomblist[i].location_x - 1 && player->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (player->location_x == player->bomblist[i].location_x + 1 && player->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (player->location_x == player->bomblist[i].location_x && player->location_y == player->bomblist[i].location_y - 1 && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1)||
          (player->location_x == player->bomblist[i].location_x && player->location_y == player->bomblist[i].location_y + 1 && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1)
          )
        {
          if (player->lifes != 0)
            {
              player->lifes--;
            }
          draw_lifes(player, lcd, 0);
        }
      if ((player->location_x == opponent->bomblist[i].location_x && player->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (player->location_x == opponent->bomblist[i].location_x - 1 && player->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (player->location_x == opponent->bomblist[i].location_x + 1 && player->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (player->location_x == opponent->bomblist[i].location_x && player->location_y == opponent->bomblist[i].location_y - 1 && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1)||
          (player->location_x == opponent->bomblist[i].location_x && player->location_y == opponent->bomblist[i].location_y + 1 && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1)
          )
        {
          if (player->lifes != 0)
            {
              player->lifes--;
              opponent->points += 250;
            }
          draw_lifes(player, lcd, 0);
          draw_lifes(opponent, lcd, 1);
        }
    }

  for (uint8_t i = 0; i < ARRAY_SIZE(opponent->bomblist); i++)
    {
      if ((opponent->location_x == opponent->bomblist[i].location_x && opponent->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == opponent->bomblist[i].location_x - 1 && opponent->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == opponent->bomblist[i].location_x + 1 && opponent->location_y == opponent->bomblist[i].location_y && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == opponent->bomblist[i].location_x && opponent->location_y == opponent->bomblist[i].location_y - 1 && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1)||
          (opponent->location_x == opponent->bomblist[i].location_x && opponent->location_y == opponent->bomblist[i].location_y + 1 && opponent->bomblist[i].exploded == 1 && opponent->bomblist[i].explosion_removed != 1)
          )
        {
          if (opponent->lifes != 0)
            {
              opponent->lifes--;
            }
          draw_lifes(opponent, lcd, 1);
        }
      if ((opponent->location_x == player->bomblist[i].location_x && opponent->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == player->bomblist[i].location_x - 1 && opponent->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == player->bomblist[i].location_x + 1 && opponent->location_y == player->bomblist[i].location_y && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1) ||
          (opponent->location_x == player->bomblist[i].location_x && opponent->location_y == player->bomblist[i].location_y - 1 && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1)||
          (opponent->location_x == player->bomblist[i].location_x && opponent->location_y == player->bomblist[i].location_y + 1 && player->bomblist[i].exploded == 1 && player->bomblist[i].explosion_removed != 1)
          )
        {
          if (opponent->lifes != 0)
            {
              opponent->lifes--;
              player->points += 250;
            }
          draw_lifes(player, lcd, 0);
          draw_lifes(opponent, lcd, 1);
        }
    }
}

void updateOpponent()
{
  // opponent pos changed, update / redraw opponent

  if (opponent->location_x > upToDateOpponentPos.location_x)
    {
      move_left(opponent, lcd);
    }
  else if (opponent->location_x < upToDateOpponentPos.location_x)
    {
      move_right(opponent, lcd);
    }

  if (opponent->location_y > upToDateOpponentPos.location_y)
    {
      move_up(opponent, lcd);
    }
  else if (opponent->location_y < upToDateOpponentPos.location_y)
    {
      move_down(opponent, lcd);
    }
  // Else, nothing to do ..
}

/*
 * This is the game loop.
 */
void gameloop(Player *player, Player *opponent, MI0283QT9 lcd)
{
  setOpponentPos(opponent->location_x, opponent->location_y);
  while (1)
    {
      lcd.led(get_ADC());
      delay(10);
      // struct buf *buffer;
      struct buf *buffer = (buf *)malloc(sizeof(struct buf));
      nunchuck_get_data(buffer);
      delay(90);
      check_if_bomb_has_to_explode(player, 0);
      check_if_bomb_has_to_explode(opponent, 1);
      check_if_player_has_to_move(player, buffer);
      updateOpponent();
      get_opponent_bombs();
      if (buffer->zButton == 1)
        {
          place_bomb(player);
        }
      if (player->lifes == 0 || opponent->lifes == 0)
        {
          stop_game();
          break;
        }
      free(buffer);
    }
}

void check_if_player_has_to_move(Player *player, struct buf *buffer)
{
  /* Move Left if Joystick to left */
  if (check_if_joystick_left(buffer))
    {
      //send position
      sendPlayerPos(player->location_x-1, player->location_y);
      move_left(player, lcd);
      //check_if_player_in_bomb_explosion();
    }
  /* Move Right if Joystick to right */
  if (check_if_joystick_right(buffer))
    {
      //send position
      sendPlayerPos(player->location_x+1, player->location_y);
      move_right(player, lcd);
      //check_if_player_in_bomb_explosion();
    }
  /* Move Up if Joystick up */
  if (check_if_joystick_up(buffer))
    {
      //send position
      sendPlayerPos(player->location_x, player->location_y-1);
      move_up(player, lcd);
      //check_if_player_in_bomb_explosion();
    }
  /* Move Down if Joystick down */
  if (check_if_joystick_down(buffer))
    {
      //send position
      sendPlayerPos(player->location_x, player->location_y+1);
      move_down(player, lcd);
      //check_if_player_in_bomb_explosion();
    }
}

void check_if_bomb_has_to_explode(Player *player, uint8_t oppontent)
{
  for (uint8_t i = 0; i < ARRAY_SIZE(player->bomblist); i++)
    {
      //check if bom has to explode
      if ( player->bomblist[i].time_placed + 3000 <=  millis() && player->bomblist[i].exploded != 1)
        {
          //bom has to explode!
          player->bomblist[i].exploded = 1;
          player->bomblist[i].isExploding = 1;
          draw_object(lcd, player->bomblist[i].location_x * BLOCKSIZE, player->bomblist[i].location_y * BLOCKSIZE, explosion_center);
          //check if wall
          if (map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x - 1] != 'w')
            {
              draw_object(lcd, (player->bomblist[i].location_x- 1) * BLOCKSIZE, player->bomblist[i].location_y * BLOCKSIZE, explosion_left);
            }
          if (map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x + 1] != 'w')
            {
              draw_object(lcd, (player->bomblist[i].location_x + 1) * BLOCKSIZE, player->bomblist[i].location_y * BLOCKSIZE, explosion_right);
            }
          if (map_arr[player->bomblist[i].location_y - 1][player->bomblist[i].location_x] != 'w')
            {
              draw_object(lcd, player->bomblist[i].location_x * BLOCKSIZE, (player->bomblist[i].location_y-1) * BLOCKSIZE, explosion_top);
            }
          if (map_arr[player->bomblist[i].location_y + 1 ][player->bomblist[i].location_x] != 'w')
            {
              draw_object(lcd, player->bomblist[i].location_x * BLOCKSIZE, (player->bomblist[i].location_y+1) * BLOCKSIZE, explosion_bottom);
            }
          //check if opjects can explosive
          if (map_arr[player->bomblist[i].location_y][player->bomblist[0].location_x - 1] == 'm')
            {
              map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x - 1] = 'n';
              player->points += 25;
            }
          if (map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x + 1] == 'm')
            {
              map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x + 1] = 'n';
              player->points += 26;
            }
          if (map_arr[player->bomblist[i].location_y - 1][player->bomblist[i].location_x] == 'm')
            {
              map_arr[player->bomblist[i].location_y - 1][player->bomblist[i].location_x] = 'n';
              player->points += 24;
            }
          if (map_arr[player->bomblist[i].location_y + 1 ][player->bomblist[i].location_x] == 'm')
            {
              map_arr[player->bomblist[i].location_y + 1 ][player->bomblist[i].location_x] = 'n';
              player->points += 23;
            }
          draw_lifes(player, lcd, oppontent);
          //check if there is a player.
          check_if_player_in_bomb_explosion();
        }
      //check if bom is exploded and animation has to be removed
      if ( player->bomblist[i].time_placed + 4000 <=  millis() && player->bomblist[i].explosion_removed != 1)
        {
          lcd.fillRect(player->bomblist[i].location_x*BLOCKSIZE,player->bomblist[i].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
          if (map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x - 1] != 'w')
            {
              lcd.fillRect((player->bomblist[i].location_x - 1)*BLOCKSIZE,player->bomblist[i].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }
          if (map_arr[player->bomblist[i].location_y][player->bomblist[i].location_x + 1] != 'w')
            {
              lcd.fillRect((player->bomblist[i].location_x + 1)*BLOCKSIZE,player->bomblist[i].location_y*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }
          if (map_arr[player->bomblist[i].location_y - 1][player->bomblist[i].location_x] != 'w')
            {
              lcd.fillRect(player->bomblist[i].location_x*BLOCKSIZE,(player->bomblist[i].location_y - 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }
          if (map_arr[player->bomblist[i].location_y + 1 ][player->bomblist[i].location_x] != 'w')
            {
              lcd.fillRect(player->bomblist[i].location_x*BLOCKSIZE,(player->bomblist[i].location_y + 1)*BLOCKSIZE,BLOCKSIZE,BLOCKSIZE,background);
            }
          player->bomblist[i].explosion_removed = 1;
          player->bomblist[i].isExploding = 0;
          draw_player(player, lcd);
          draw_player(opponent, lcd);
        }
    }
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
void move_left(Player *player,MI0283QT9 lcd)
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
    }
}

/*
 * This function moves a player one position to the right.
 */
void move_right(Player *player,MI0283QT9 lcd)
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
    }
}

/*
 * This function moves a player one position down.
 */
void move_down(Player *player,MI0283QT9 lcd)
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
    }
}

/*
 * This function moves a player one position up.
 */
void move_up(Player *player,MI0283QT9 lcd)
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
    }
}

ISR(TIMER2_OVF_vect)
{
}
