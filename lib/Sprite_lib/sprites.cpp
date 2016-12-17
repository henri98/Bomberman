#include <sprites.h>

void draw_object(MI0283QT9 lcd, int x, int y, const char sprite[16][16])
{
  for (int spritex = 0; spritex < 16; spritex++)
    {
      for (int spritey = 0; spritey < 16; spritey++)
        {
          switch (pgm_read_byte(&(sprite[spritey][spritex])))
            {
            case 'c':
              //nothing
              break;
            case 'b':
              lcd.drawPixel(x+spritex, y+spritey, black);
              break;
            case 'w':
              lcd.drawPixel(x+spritex, y+spritey, white);
              break;
            case 'r':
              lcd.drawPixel(x+spritex, y+spritey, red);
              break;
            case '@':
              lcd.drawPixel(x+spritex, y+spritey, grey);
              break;
            case '#':
              lcd.drawPixel(x+spritex, y+spritey, grey1);
              break;
            case 'q':
              lcd.drawPixel(x+spritex, y+spritey, yellow);
              break;
            case 'a':
              lcd.drawPixel(x+spritex, y+spritey, yellow1);
              break;
            case 'z':
              lcd.drawPixel(x+spritex, y+spritey, orange);
              break;
            case '1':
              lcd.drawPixel(x+spritex, y+spritey, white1);
              break;
            case '2':
              lcd.drawPixel(x+spritex, y+spritey, blue);
              break;
            }
        }
    }
}

void draw_player_sprite(MI0283QT9 lcd, int x, int y, uint_least16_t c, const char sprite[24][16])
{
  for (int spritex = 0; spritex < 16; spritex++)
    {
      for (int spritey = 0; spritey < 24; spritey++)
        {
          switch (pgm_read_byte(&(sprite[spritey][spritex])))
            {
            case 'c':
              //nothing
              break;
            case 'b':
              lcd.drawPixel(x+spritex, y+spritey, black);
              break;
            case 'w':
              lcd.drawPixel(x+spritex, y+spritey, white);
              break;
            case 'r':
              lcd.drawPixel(x+spritex, y+spritey, red);
              break;
            case '@':
              lcd.drawPixel(x+spritex, y+spritey, grey);
              break;
            case '#':
              lcd.drawPixel(x+spritex, y+spritey, grey1);
              break;
            case 'q':
              lcd.drawPixel(x+spritex, y+spritey, yellow);
              break;
            case 'a':
              lcd.drawPixel(x+spritex, y+spritey, yellow1);
              break;
            case 'z':
              lcd.drawPixel(x+spritex, y+spritey, orange);
              break;
            case '1':
              lcd.drawPixel(x+spritex, y+spritey, white1);
              break;
            case '2':
              lcd.drawPixel(x+spritex, y+spritey, c);
              break;
            }
        }
    }
}
