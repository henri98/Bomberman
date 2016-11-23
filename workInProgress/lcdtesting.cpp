#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <Nunchuck.h>

//MI0283QT9 Adapter v1
MI0283QT9 lcd;

int main(int argc, char const *argv[])
{
	int centerX = 128;
	int centerY = 134;
	int currentX = 20;
	int currentY = 20;
	int oldX;
	int oldY;

	// init arduino library
	init();

	// init nunchuck 
	nunchuck_init();

	// start serial
	Serial.begin(9600);

	//init display 320*240
	lcd.begin();
	lcd.led(100);

	//clear screen
	lcd.fillScreen(RGB(0, 0, 0));


	lcd.fillRect(20, 20, 20, 20, RGB(0, 0, 0));




	while (true)
	{
		// struct buf *buffer;
		struct buf *buffer = (buf *)malloc(sizeof(struct buf));

		nunchuck_get_data(buffer);

		Serial.print(buffer->zButton);

		Serial.print("\t");
		Serial.print(buffer->cButton);

		Serial.print("\t");
		Serial.print(buffer->xJoystick);

		Serial.print("\t");
		Serial.print(buffer->yJoystick);

		// Serial.print("\t");
		// Serial.print(buffer->xAccel);
		//
		// Serial.print("\t");
		// Serial.print(buffer->yAccel);
		//
		// Serial.print("\t");
		// Serial.print(buffer->zAccel);

		//rechts
		if (buffer->xJoystick > centerX + 20 && currentX < 280)
		{

			lcd.drawLine(currentX + 20, currentY, currentX + 20, currentY + 19, RGB(255, 0, 0));
			lcd.drawLine(currentX, currentY, currentX, currentY + 19, RGB(0, 0, 0));
			currentX = currentX + 1;
		}

		// links
		if (buffer->xJoystick < centerX - 20 && currentX > 20)
		{

			lcd.drawLine(currentX, currentY, currentX, currentY + 19, RGB(0, 255, 0));
			lcd.drawLine(currentX + 19, currentY, currentX + 19, currentY + 19, RGB(0, 0, 0));
			currentX = currentX - 1;
		}

		// omlaag
		if (buffer->yJoystick < centerY - 20 && currentY < 200)
		{
			lcd.drawLine(currentX, currentY + 20, currentX + 19, currentY + 20, RGB(0, 0, 255));
			lcd.drawLine(currentX, currentY, currentX + 19, currentY, RGB(0, 0, 0));
			currentY = currentY + 1;
		}

		// omhoog
		if (buffer->yJoystick > centerY + 20 && currentY > 20)
		{
			lcd.drawLine(currentX, currentY - 1, currentX + 19, currentY - 1, RGB(0, 255, 255));
			lcd.drawLine(currentX, currentY + 19, currentX + 19, currentY + 19, RGB(0, 0, 0));

			currentY = currentY - 1;
		}


		/*	for (int i = 0; i <= 300; i++)
			{
				lcd.drawLine(i + 20, 20, i + 20, 39, RGB(0, 0, 0));
				lcd.drawLine(i, 20, i, 39, RGB(255, 255, 255));
			}

			for (int i = 300; i >= 0; i--)
			{
				lcd.drawLine(i + 20, 20, i + 20, 39, RGB(255, 255, 255));
				lcd.drawLine(i, 20, i, 39, RGB(0, 0, 0));
			}*/

		Serial.println("");
		free(buffer);

	};
}


