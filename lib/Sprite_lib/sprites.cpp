#include <sprites.h>

void draw_object(MI0283QT9 lcd, int x, int y, char sprite[16][16])
{
  lcd.setOrientation(90);
  for (int x = 0; x <= WIDTH; x++)
  {
    for (int y = 0; y <= HEIGHT; y++)
    {
      switch (sprite[x][y]) {
        case 'c':
        lcd.drawPixel(x, y + 320 - 16, background);
        break;
        case 'b':
        lcd.drawPixel(x, y + 320 - 16, black);
        break;
        case 'w':
        lcd.drawPixel(x, y + 320 - 16, white);
        break;
        case 'r':
        lcd.drawPixel(x, y + 320 - 16, red);
        break;
      }
    }
  }
}
