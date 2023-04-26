#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <Arduino.h>

typedef struct ConfigureMessage
{
    char sensor;
    bool on;
    bool highPriority;
    word lowerThreshold;
    word upperThreshold;
    char interval;
};

typedef enum Sensors
{
    PHOTODIODE = 1,  // value: 0-1023, 10 bit unsigned int
    TEMPERATURE = 2, // value: 0-4094 ^= -204.7 to +204.7, 12 bit signed fixed point
    HUMIDITY = 3,    // value: 0-100, 7 bit unsigned int
    MICROSWITCH = 4, // value: 0-1, 1 bit unsigned int
    ACCELERATION = 5 // value: 0-8190 ^= -409.5 to +409.5, 13 bit signed fixed point
};

void charArrayToHexString(char *charArray, int length, char *hexString);
String encodeAlertMessage(sensors sensor, bool priority, bool toohigh, word value);
bool receiveAlertMessage(char *msg, sensors *sensor, bool *priority, bool *toohigh, word *value);
configureMessage decodeConfigMessage(char msg[6]);
String decodeAlertMessage(char *msg);
int hexToInt(char c);

#endif // PROTOCOL_H
