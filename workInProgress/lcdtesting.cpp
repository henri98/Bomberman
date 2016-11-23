#include <GraphicsLib.h>
#include <MI0283QT9.h>

//MI0283QT9 Adapter v1
MI0283QT9 lcd;

int main(int argc, char const *argv[])
{
	int Xa;
	int Xb;
	int Ya;
	int Yb;

	// init arduino library
	init();

	//init display 320*240
	lcd.begin();
	lcd.led(100);

	//clear screen
	lcd.fillScreen(RGB(255, 255, 255));

	
	lcd.fillRect(0, 20, 20, 20, RGB(0, 0, 0));
	
	


	while (true)
	{
		for (int i = 0; i <= 300; i++)
		{

			lcd.drawLine(i + 20, 20, i + 20, 39, RGB(0, 0, 0));
			lcd.drawLine(i, 20, i, 39, RGB(255, 255, 255));

			lcd.drawLine(i + 20, 60, i + 20, 79, RGB(0, 0, 0));
			lcd.drawLine(i, 60, i, 79, RGB(255, 255, 255));

			lcd.drawLine(i + 20, 100, i + 20, 119, RGB(0, 0, 0));
			lcd.drawLine(i, 100, i, 119, RGB(255, 255, 255));

			lcd.drawLine(i + 20, 140, i + 20, 159, RGB(0, 0, 0));
			lcd.drawLine(i, 140, i, 159, RGB(255, 255, 255));

			lcd.drawLine(i + 20, 180, i + 20, 199, RGB(0, 0, 0));
			lcd.drawLine(i, 180, i, 199, RGB(255, 255, 255));

			delay(1);
		}

		for (int i = 300; i >= 0; i--)
		{
			lcd.drawLine(i + 20, 20, i + 20, 39, RGB(255, 255, 255));
			lcd.drawLine(i, 20, i, 39, RGB(0, 0, 0));

			lcd.drawLine(i + 20, 60, i + 20, 79, RGB(255, 255, 255));
			lcd.drawLine(i, 60, i, 79, RGB(0, 0, 0));

			lcd.drawLine(i + 20, 100, i + 20, 119, RGB(255, 255, 255));
			lcd.drawLine(i, 100, i, 119, RGB(0, 0, 0));

			lcd.drawLine(i + 20, 140, i + 20, 159, RGB(255, 255, 255));
			lcd.drawLine(i, 140, i, 159, RGB(0, 0, 0));

			lcd.drawLine(i + 20, 180, i + 20, 199, RGB(255, 255, 255));
			lcd.drawLine(i, 180, i, 199, RGB(0, 0, 0));

			delay(1);
		}

	};
}
