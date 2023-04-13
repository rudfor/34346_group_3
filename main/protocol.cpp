#include "protocol.h"

bool sendAlertMessage(sensors sensor, bool priority, bool toohigh, word value)
{
    char first = (sensor & 0b111) | ((priority & 0b1) << 3) | ((toohigh & 0b1) << 4);
    char msg[3] = {first, (value >> 8) & 0xFF, value & 0xFF};
    // send the message
    return true;
}
configureMessage decodeConfigMessage(char msg[6])
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
    configureMessage cmsg = {sensor, on, highPriority, (msg[1] << 8) | msg[2], (msg[3] << 8) | msg[4], msg[5]};
    return cmsg;
}
