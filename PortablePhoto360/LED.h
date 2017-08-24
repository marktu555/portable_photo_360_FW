#ifndef __LED_H__
#define __LED_H__


#define LED_PIN_0 3
#define LED_PIN_1 9
//#define LED_PIN_2

typedef struct led_config {
    unsigned short led_0;
    unsigned short led_1;
    unsigned short led_2;
} LED_CONFIG;

void LED_init();
void LED_set(LED_CONFIG *conf);
/*
class LED : public LED_init
{
public:
    void LED_set(LED_CONFIG &conf);

protected:

private:

};*/

#endif
