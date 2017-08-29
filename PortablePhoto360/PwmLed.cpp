#include "PwmLed.h"
// Import Arduino.h for analogWrite()
#include "Arduino.h"

PwmLed::PwmLed(void)
{
    // cannot setup GPIO here
    analogWrite(LED_PIN_0, 0);
    analogWrite(LED_PIN_1, 0);
}

void PwmLed::LED_set(unsigned short led_0, unsigned short led_1, unsigned short led_2)
{
    analogWrite(LED_PIN_0, led_0);
    analogWrite(LED_PIN_1, led_1);
    //analogWrite(LED_PIN_2, led_2);
}
