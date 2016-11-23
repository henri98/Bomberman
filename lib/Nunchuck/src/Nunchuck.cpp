#include "Nunchuck.h"

void nunchuck_request();
char nunchuck_decode_byte(char x);

// Setup I2C
// "Handshake" with nunchuck
void nunchuck_init()
{
  Wire.begin();
  Wire.beginTransmission(NUNCHUCK_ADDR);

  Wire.write((uint8_t)0x40);
  Wire.write((uint8_t)0x00);

  Wire.endTransmission();
}

// Requests data from nunchuck
void nunchuck_request()
{
  Wire.beginTransmission(NUNCHUCK_ADDR);
  Wire.write((uint8_t)0x00); // Sending a 0 requests data
  Wire.endTransmission();
}

// Decode the nunchuck data
char nunchuck_decode_byte(char x)
{
  return (x ^ 0x17) + 0x17;
}

// Request the data from the nunchuck and
// put it in the given struct
bool nunchuck_get_data(struct buf *buffer)
{
  uint8_t count = 0;
  Wire.requestFrom(NUNCHUCK_ADDR, 6);

  while (Wire.available())
  {

    switch (count)
    {

#if USE_NUNCHUCK_JOYSTICK
      case 0: // X Joystick
        buffer->xJoystick = nunchuck_decode_byte(Wire.read());
        break;

      case 1: // Y Joystick
        buffer->yJoystick = nunchuck_decode_byte(Wire.read());
        break;
#endif

#if USE_NUNCHUCK_MOVEMENT
      case 2: // X Acceleration
        buffer->xAccel = nunchuck_decode_byte(Wire.read());
        break;

      case 3: // Y Acceleration
        buffer->yAccel = nunchuck_decode_byte(Wire.read());
        break;

      case 4: // Z Acceleration
        buffer->zAccel = nunchuck_decode_byte(Wire.read());
        break;
#endif

      case 5: // C / Z Buttons
      {
        char tempChar = (char) nunchuck_decode_byte(Wire.read());
        buffer->zButton = ((tempChar >> 0) & 1) ? false : true;
        buffer->cButton = ((tempChar >> 1) & 1) ? false : true;
      }
        break;

      default:
        Wire.read();
        break;
    }

    count++;
  }

  nunchuck_request(); // Request new data

  return (count <= 5) ? true : false; // Success / Fail
}
