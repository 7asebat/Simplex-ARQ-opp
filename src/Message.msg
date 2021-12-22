struct Event
{
    uint8_t error;
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
    string payload;
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
