#include "LGATTSUart.h"
#include <LGATTUUID.h>
#include "Stepper.h"

#ifdef APP_LOG
#undef APP_LOG
#endif

#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();

// declare variable
const int stepsPerRevolution = 2053;
LGATTSUart uart;
MOTOR_CONFIG motor;
LED_CONFIG led;
static int steps = 0;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);    // initialize the stepper library on pins 8 through 11:

void Proc_Motor_Ctrl(){
    if(motor.received == 1){
        motor.received = 0;
        APP_LOG("motor received flag assert: %2x %2x", motor.rotate, motor.angle);

        if(motor.rotate == 0x01) {
            //APP_LOG("clockwise");
            steps = motor.angle;
        } else {
            //APP_LOG("counterclockwise");
            steps = -motor.angle;
        }
        myStepper.step(steps);
    }

}
void setup() {
    // put your setup code here, to run once:
    myStepper.setSpeed(10);     // set the speed at 20 rpm:

    Serial.begin(115200);
    delay(2000);

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


