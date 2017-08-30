#include "vmbtgatt.h"
#include "LGATTSUart.h"
#include "PwmLed.h"

#ifdef APP_LOG
#undef APP_LOG
#define APP_LOG(...) Serial.printf(__VA_ARGS__);
#define APP_LOGLN(...) Serial.printf(__VA_ARGS__); \
    Serial.println();
#endif

#define ATTR_GEN  258
#define ATTR_LED  260
#define ATTR_MOT  262

extern MOTOR_CONFIG motor;
PwmLed led;
LED_CONFIG led_config;

/*
 * typedef struct {
    ard_decl_type_enum type;
    uint8_t uuid[37];
    uint8_t isprimary;
    LGATT_CHAR_PROPERTIES prop;
    LGATT_PERMISSION permission;
    int32_t triggered;
  } LGATTServiceInfo;
 */
static LGATTServiceInfo g_uart_decl[] =
{
    {TYPE_SERVICE, "E20A39F4-73F5-4BC4-A12F-17D1AD07A961", TRUE, 0, 0, 0},
    {TYPE_CHARACTERISTIC, "01234567-89AB-CDEF-0123-456789ABCDE0", FALSE, 
        VM_GATT_CHAR_PROP_READ, VM_GATT_PERM_READ, 0}, // tx for periphral; rx for central
    {TYPE_CHARACTERISTIC, "01234567-89AB-CDEF-0123-456789ABCDE1", FALSE, 
        VM_GATT_CHAR_PROP_READ | VM_GATT_CHAR_PROP_WRITE | VM_GATT_CHAR_PROP_INDICATE, VM_GATT_PERM_READ | VM_GATT_PERM_WRITE, 0}, // rx for periphral; tx for central
    {TYPE_CHARACTERISTIC, "01234567-89AB-CDEF-0123-456789ABCDE2", FALSE, 
        VM_GATT_CHAR_PROP_READ | VM_GATT_CHAR_PROP_WRITE | VM_GATT_CHAR_PROP_INDICATE, VM_GATT_PERM_READ | VM_GATT_PERM_WRITE, 0}, // rx for periphral; tx for central
    {TYPE_END, 0, 0, 0, 0, 0}
};

uint16_t LGATTSUart::getHandle(int32_t type)
{
    APP_LOGLN("LGATTSUart::getHandle: %d", type);
    if (0 == type)
    {
        return _handle_notify;
    }
    else if (1 == type)
    {
        return _handle_write;
    }

    return 0;
}

// prepare the data for profile
LGATTServiceInfo *LGATTSUart::onLoadService(int32_t index)
{
    //LED_init();
    return g_uart_decl;
}

// characteristic added
boolean LGATTSUart::onCharacteristicAdded(LGATTAttributeData &data)
{
    const VM_BT_UUID *uuid = &(data.uuid);
    APP_LOGLN("LGATTSUart::onCharacteristicAdded f[%d] uuid[0] = [0x%x] len[%d]", data.failed, uuid->uuid[0], uuid->len);
    if (!data.failed)
    {
        if (0xE0 == uuid->uuid[0])
        {
            _handle_notify = data.handle;
        }
        else if (0xE1 == uuid->uuid[0])
        {
            _handle_write = data.handle;
        }
        
    }
    return true;
}

// connected or disconnected
boolean LGATTSUart::onConnection(const LGATTAddress &addr, boolean connected)
{
    _connected = connected;
    APP_LOGLN("LGATTSUart::onConnection connected [%d], [%x:%x:%x:%x:%x:%x]", _connected, 
        addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0]);

    return true;
}

// read action comming from master
boolean LGATTSUart::onRead(LGATTReadRequest &data)
{
    APP_LOGLN("LGATTSUart::onRead _connected [%d][%d]", _connected, data.attr_handle);
    if (_connected)
    {
        LGATTAttributeValue value = {0};
#if 0
        const char *str = "[peripheral] tx";
        memcpy(value.value, str, strlen(str));
        value.len = strlen(str);
        APP_LOG("LGATTSUart::onRead onRead [%d][%s]", value.len, value.value);
#else
        switch (data.attr_handle)
        {
            case ATTR_GEN:
                value.value[0] = 0x00;
                value.len = 1;
                break;
            case ATTR_LED:
                value.value[0] = led_config.led_0 & 0xFF;
                value.value[1] = led_config.led_0 >> 8;
                value.value[2] = led_config.led_1 & 0xFF;
                value.value[3] = led_config.led_1 >> 8;
                value.value[4] = led_config.led_2 & 0xFF;
                value.value[5] = led_config.led_2 >> 8;
                value.len = 6;
                break;
            case ATTR_MOT:
                value.value[0] = motor.busy;
                value.value[1] = motor.rotate;
                value.value[2] = motor.angle;
                value.len = 3;
                break;
            default:
                APP_LOG("Unknown handle");
        }
        APP_LOGLN("LGATTSUart::onRead onRead [%d]", value.len);
        for(int i=0; i<value.len; i++) {
            APP_LOG("[%02X]", value.value[i]);
        }
        APP_LOGLN("");
        
#endif
        data.ackOK(value);
        
    }
    return true;
}

// write action comming from master
boolean LGATTSUart::onWrite(LGATTWriteRequest &data)
{
    APP_LOGLN("LGATTSUart::onWrite _connected [%d][%d]", _connected, data.attr_handle);
    // Read UART data.
    if (_connected)
    {
        LGATTAttributeValue value;
        // if need to rsp to central.
        if (data.need_rsp)
        {
            value.len = 0;
            data.ackOK();
        }

        /*
        if (data.offset + data.value.len <= ATT_MAX_VALUE_LEN)
        {
            _value.len = data.value.len;
            memcpy(&_value.value, data.value.value + data.offset, data.value.len);
        }
        */
        APP_LOGLN("central data on peripheral rx[%d]", data.value.len);
        for(int i=0; i<data.value.len; i++) {
            APP_LOG("[%02X]", data.value.value[i]);
        }
        APP_LOGLN("");
        switch (data.attr_handle)
        {
            case ATTR_LED:
                led_config.led_0 = data.value.value[0] | (data.value.value[1] << 8);
                led_config.led_1 = data.value.value[2] | (data.value.value[3] << 8);
                led_config.led_2 = data.value.value[4] | (data.value.value[5] << 8);
                APP_LOGLN("Set LED: %d %d %d", led_config.led_0, led_config.led_1, led_config.led_2);
                led.LED_set(led_config.led_0, led_config.led_1, led_config.led_2);
                break;
            case ATTR_MOT:
                motor.rotate = data.value.value[1] & 0x01;
                motor.angle = data.value.value[2] & 0xFF;
                motor.busy = 1;
                APP_LOGLN("Set Motor %2X %2X", motor.rotate, motor.angle);
                break;
            default:
                APP_LOG("Unknown handle");
        }
    }
    return true;
}

