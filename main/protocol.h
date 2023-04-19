#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <Arduino.h>

typedef struct configureMessage
{
    char sensor;
    bool on;
    bool highPriority;
    word lowerThreshold;
    word upperThreshold;
    char interval;
};

enum sensors
{
    PHOTODIODE = 1,  // value: 0-1023, 10 bit unsigned int
    TEMPERATURE = 2, // value: 0-4095 ^= -204.7 to +204.7, 12 bit signed fixed point
    HUMIDITY = 3,    // value: 0-100, 7 bit unsigned int
    MICROSWITCH = 4, // value: 0-1, 1 bit unsigned int
    ACCELERATION = 5 // value: 0-8192 ^= -409.5 to +409.5, 13 bit signed fixed point
};

bool sendAlertMessage(sensors sensor, bool priority, bool toohigh, word value);
configureMessage decodeConfigMessage(char msg[6]);

#endif // PROTOCOL_H
