#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <Nunchuck.h>

// MI0283QT9 Adapter v1
MI0283QT9 lcd;

int main(int argc, char const *argv[])
{
	int centerX = 128;
	int centerY = 134;
	int currentX = 150;
	int currentY = 110;

	// init arduino library
	init();

	// init nunchuck 
	nunchuck_init();

	// start serial
	Serial.begin(9600);

	//init display 320*240
	lcd.begin();
	lcd.led(100);

	// clear screen
	lcd.fillScreen(RGB(0, 0, 0));

	// black square startposition
	lcd.fillRect(150, 110, 20, 20, RGB(255, 0, 0));

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


		// controller to left
		if (buffer->xJoystick < centerX - 40 && currentX > 0)
		{
			lcd.drawLine(currentX - 1, currentY, currentX - 1, currentY + 19, RGB(255, 0, 0));
			lcd.drawLine(currentX + 19, currentY, currentX + 19, currentY + 19, RGB(0, 0, 0));
			currentX = currentX - 1;
		}

		// controller to right
		if (buffer->xJoystick > centerX + 40 && currentX < 300)
		{
			lcd.drawLine(currentX + 20, currentY, currentX + 20, currentY + 19, RGB(255, 0, 0));
			lcd.drawLine(currentX, currentY, currentX, currentY + 19, RGB(0, 0, 0));
			currentX = currentX + 1;
		}

		// controller up
		if (buffer->yJoystick > centerY + 40 && currentY > 0)
		{
			lcd.drawLine(currentX, currentY - 1, currentX + 19, currentY - 1, RGB(255, 0, 0));
			lcd.drawLine(currentX, currentY + 19, currentX + 19, currentY + 19, RGB(0, 0, 0));
			currentY = currentY - 1;
		}

		// controller down
		if (buffer->yJoystick < centerY - 40 && currentY < 220)
		{
			lcd.drawLine(currentX, currentY + 20, currentX + 19, currentY + 20, RGB(255, 0, 0));
			lcd.drawLine(currentX, currentY, currentX + 19, currentY, RGB(0, 0, 0));
			currentY = currentY + 1;
		}

		Serial.println("");

		free(buffer);
	};
}


