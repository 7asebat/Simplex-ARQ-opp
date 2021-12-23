#pragma once
#include <omnetpp.h>

using namespace omnetpp;

using byte = uint8_t;
using string = opp_string;

enum Error_Type : uint8_t
{
	ERROR_TYPE_NONE         = 0,
	ERROR_TYPE_DELAY        = 1 << 0,
	ERROR_TYPE_DUPLICATION  = 1 << 1,
	ERROR_TYPE_LOSS         = 1 << 2,
	ERROR_TYPE_MODIFICATION = 1 << 3,

    EVENT_ACK_TIMEOUT      = 1 << 4,
    EVENT_ACK              = 1 << 5,
    EVENT_NACK             = 1 << 6,
    EVENT_DUPLICATE_FRAME  = 1 << 7,
};

inline static const char*
error_type_to_c_str(uint8_t error)
{
    static char buf[1024];

    switch (error)
    {
    case ERROR_TYPE_NONE:       return "No Errors";
    case EVENT_ACK_TIMEOUT:     return "Internal: Timeout";
    case EVENT_ACK:             return "Internal: ACK";
    case EVENT_NACK:            return "Internal: NACK";
    case EVENT_DUPLICATE_FRAME: return "Internal: Duplicating Frame";

    default: strcpy(buf, "Errors:");
    }

    if (error & ERROR_TYPE_MODIFICATION)
        strcat(buf, " Modification");
    if (error & ERROR_TYPE_LOSS)
        strcat(buf, " Loss");
    if (error & ERROR_TYPE_DUPLICATION)
        strcat(buf, " Duplication");
    if (error & ERROR_TYPE_DELAY)
        strcat(buf, " Delay");

    return buf;
}

inline static const char *
message_type_to_c_str(uint8_t type)
{
    switch (type)
    {
        case 0: return "DATA";
        case 1: return "ACK";
        case 2: return "NACK";
    }
    return nullptr;
}
