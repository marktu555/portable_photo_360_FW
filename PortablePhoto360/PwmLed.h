#ifndef PwmLed_h
#define PwmLed_h

#define LED_PIN_0 3
#define LED_PIN_1 9
//#define LED_PIN_2

typedef struct led_config {
    unsigned short led_0;
    unsigned short led_1;
    unsigned short led_2;
} LED_CONFIG;

class PwmLed {
public:
    PwmLed(void);
    void LED_set(unsigned short, unsigned short, unsigned short);
};

#endif