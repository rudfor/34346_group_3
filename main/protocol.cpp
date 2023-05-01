#include "protocol.h"

void charArrayToHexString(char *charArray, int length, char *hexString)
{
    const char *hexChars = "0123456789ABCDEF";
    for (int i = 0; i < length; ++i)
    {
        hexString[i * 2] = hexChars[(charArray[i] >> 4) & 0xF];
        hexString[i * 2 + 1] = hexChars[charArray[i] & 0xF];
    }
    hexString[length * 2] = '\0';
}

String encodeAlertMessage(Sensors sensor, bool priority, bool toohigh, word value)
{
    char first = (sensor & 0b111) | ((priority & 0b1) << 3) | ((toohigh & 0b1) << 4);
    char msg[3] = {first, (value >> 8) & 0xFF, value & 0xFF};
    const char *hexChars = "0123456789ABCDEF";
    String hexString = "";
    for (int i = 0; i < 3; ++i)
    {
        hexString += hexChars[(msg[i] >> 4) & 0xF];
        hexString += hexChars[msg[i] & 0xF];
    }
    return hexString;
}

bool receiveAlertMessage(char *msg, Sensors *sensor, bool *priority, bool *toohigh, word *value)
{
    if (!msg)
    {
        return false;
    }

    // Extract values from the message
    char first = msg[0];
    *sensor = (Sensors)(first & 0b111);
    *priority = (first >> 3) & 0b1;
    *toohigh = (first >> 4) & 0b1;
    *value = ((word)msg[1] << 8) | msg[2];

    return true;
}

ConfigureMessage decodeConfigMessage(char msg[6])
{
    char sensor = msg[0] & 0b111;
    bool state = (msg[0] >> 3) & 0b11;
    bool on = false;
    bool highPriority = false;
    if (state != 0)
    {
        on = true;
        if (state == 3)
        {
            highPriority = true;
        }
    }
    ConfigureMessage cmsg = {sensor, on, highPriority, (msg[1] << 8) | msg[2], (msg[3] << 8) | msg[4], msg[5]};
    return cmsg;
}

String decodeAlertMessage(char *msg)
{
    char sensor = msg[0] & 0b111;
    bool priority = (msg[0] >> 3) & 0b1;
    bool toohigh = (msg[0] >> 4) & 0b1;
    word value = (msg[1] << 8) | msg[2];

    String sensorName;
    float decodedValue;
    if (sensor == 1)
    {
        sensorName = "PhotoDiode";
        decodedValue = value;
    }
    else if (sensor == 2)
    {
        sensorName = "Temperature";
        decodedValue = (value - 2047) / 10.0;
    }
    else if (sensor == 3)
    {
        sensorName = "humidity";
        decodedValue = value;
    }
    else if (sensor == 4)
    {
        sensorName = "microswitch";
        decodedValue = value;
    }
    else if (sensor == 5)
    {
        sensorName = "n3AxisAccelerometer";
        decodedValue = (value - 4095) / 10.0;
    }
    else
    {
        return "{\"status\":\"error\"}";
    }

    // Construct JSON object
    String json = "{\"" + sensorName + "\":" + String(decodedValue, 2) + "}";
    // Return JSON as a string
    return json;
}
