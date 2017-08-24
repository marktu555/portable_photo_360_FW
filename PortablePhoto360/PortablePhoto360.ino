#include "LGATTSUart.h"
#include <LGATTUUID.h>

#ifdef APP_LOG
#undef APP_LOG
#endif

#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();

#define MOTOR_PIN_EN    5
#define MOTOR_PIN_DIR   6
#define MOTOR_PIN_STEP  7
#define MOTOR_CIRCLE_PER_STEPS  8192
#define DEGREE_TO_STEPS  (MOTOR_CIRCLE_PER_STEPS/360) //->22  should be 22.75

// Declare variable
LGATTSUart uart;
MOTOR_CONFIG motor;

static int steps = 0;

void Proc_Motor_Ctrl(){
    if(motor.busy == 1){
        APP_LOG("motor received flag assert: %2x %2x(%d)", motor.rotate, motor.angle, motor.angle);

        if(motor.rotate == 0x01) {
            //APP_LOG("clockwise");
            digitalWrite(MOTOR_PIN_DIR, HIGH);
        } else {
            //APP_LOG("counterclockwise");
            digitalWrite(MOTOR_PIN_DIR, LOW);
        }
        // Convert angle to steps
        steps =  motor.angle * DEGREE_TO_STEPS;
        APP_LOG("%d steps", steps);

        for(int i = 0; i<steps; i++) {
            digitalWrite(MOTOR_PIN_STEP,HIGH);
            delayMicroseconds(100);
            digitalWrite(MOTOR_PIN_STEP,LOW);
            delayMicroseconds(100);
        }
        motor.busy = 0;
    }
}

void setup() {
    // put your setup code here, to run once:
    pinMode(MOTOR_PIN_EN, OUTPUT);
    pinMode(MOTOR_PIN_DIR, OUTPUT);
    pinMode(MOTOR_PIN_STEP, OUTPUT);

    digitalWrite(MOTOR_PIN_EN, HIGH); // disable first

    Serial.begin(115200);
    delay(2000);
    APP_LOG("1 degree cost %d steps", DEGREE_TO_STEPS);

    if (!LGATTServer.begin(1, &uart))
    {
        APP_LOG("[FAILED] GATTS begin");
    }
    else
    {
        APP_LOG("GATTS begin");
    }
    
}

void loop() {
    // put your main code here, to run repeatedly:
    delay(1000);
    LGATTServer.handleEvents();
    //LGATTAttributeValue value = {13, "hello, world!"};
    Proc_Motor_Ctrl();

#if 0
    if (uart.isConnected())
    {
        LGATTAttributeValue value = {0};
        const char *str = "[peripheral] hello";
        value.len = strlen(str);
        memcpy(value.value, str, value.len);
        boolean ret = uart.sendIndication(value, uart.getHandleNotify(), false);
        if (!ret)
        {
            APP_LOG("[FAILED] send [%d]", ret);
        }
        APP_LOG("send [%d][%s]", ret, str);
    }
    //uart.send(value, uart.getHandle(0), false); // just a notify
#endif
}

