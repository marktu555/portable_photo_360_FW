#include "LED.h"

void LED_init()
{
    analogWrite(LED_PIN_0, 0);
    analogWrite(LED_PIN_1, 0);
}

void LED_set(LED_CONFIG* conf)
{
    analogWrite(LED_PIN_0, conf->led_0);
    analogWrite(LED_PIN_1, conf->led_1);
    //analogWrite(LED_PIN_2, conf->led_2);
}
