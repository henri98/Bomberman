#include "brightness.h"

double brightness;

void init_ADC()
{
  ADCSRA |= (1 << ADPS0, 1 << ADPS1, 1 << ADPS2);
  ADCSRA |= (1 << ADEN);
  ADMUX |= (1 << REFS0);
}

int get_ADC()
{
  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  brightness = ADC;
  brightness = brightness / 1023 * 100;
  return brightness;
}
