#include <Wire.h>

// Toggles if the movement-data of the Nunchuck is read
#define USE_NUNCHUCK_MOVEMENT false

// Toggles if the joystick-data of the Nunchuck is read
#define USE_NUNCHUCK_JOYSTICK true

#define NUNCHUCK_ADDR 0x52 // The Nunchuck's address

typedef struct buf {
    bool zButton;
    bool cButton;

#if USE_NUNCHUCK_JOYSTICK
    uint8_t xJoystick;
    uint8_t yJoystick;
#endif

#if USE_NUNCHUCK_MOVEMENT
    uint8_t xAccel;
    uint8_t yAccel;
    uint8_t zAccel;
#endif

} buf;

void nunchuck_init();
bool nunchuck_get_data(struct buf *buffer);
