cplusplus {{
    #include <ARQ.h>
}}

class noncobject Payload;

struct Event
{
    uint8_t error;
    uint8_t message_id;
    string content;
}

enum Message_Type
{
    DATA = 0;
    ACK  = 1;
    NACK = 2;
};

struct Header
{
    uint8_t message_id;
    double sending_time;
	uint8_t message_type @enum(Message_Type);
}

struct Frame
{
    Header header;
    Payload payload;
	uint8_t trailer;
}

packet Message_Event
{
    Event event;
}

packet Message_Frame
{
    Frame frame;
}

packet Message_Duplicate_Frame
{
    Frame frame;
    bool lost;
}