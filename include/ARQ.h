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
    const int SIZE = 4;
    static char binary_str[SIZE+1] = {};

    for (int i = 0; i < SIZE; i++)
    {
        binary_str[SIZE-1 - i] = (error >> i) & 1 ? '1' : '0';
    }

    switch (error)
    {
    case ERROR_TYPE_NONE:         return "No Errors";
    case ERROR_TYPE_DELAY:        return "Error: Delay";
    case ERROR_TYPE_LOSS:         return "Error: Loss";
    case ERROR_TYPE_MODIFICATION: return "Error: Modification";
    case ERROR_TYPE_DUPLICATION:  return "Error: Duplication";

    case EVENT_ACK_TIMEOUT:     return "Internal: Timeout";
    case EVENT_ACK:             return "Internal: ACK";
    case EVENT_NACK:            return "Internal: NACK";
    case EVENT_DUPLICATE_FRAME: return "Internal: Duplicating Frame";

    default:
        return binary_str;
    }
    return nullptr;
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
