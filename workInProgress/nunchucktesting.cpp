#include <Nunchuck.h>
#include <Arduino.h>

int main(int argc, char const *argv[])
{
  init();
  Serial.begin(9600);

  nunchuck_init();

  while (true) {
    delay(100);

    // struct buf *buffer;
    struct buf *buffer = (buf *) malloc(sizeof(struct buf));

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

    free(buffer);

    Serial.println("");
  }

  for(;;);
}
