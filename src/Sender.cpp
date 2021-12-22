//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <regex>

#include <Sender.h>

Define_Module(Sender);

void Sender::initialize()
{
    num_correct_messages = 0;
    num_total_messages = 0;
    transmission_time = 0;
    frame_id_to_send = 0;

    start_time = par("start_time").doubleValue();
    timeout_interval = par("timeout_interval").doubleValue();
    packet_delay = par("packet_delay").doubleValue();
    input_filepath = par("input_filepath").stringValue();

    auto output_filepath = getParentModule()->par("output_filepath").stringValue();
    out_f.open(output_filepath, out_f.out | out_f.app);

    load_input();
    schedule_event(events[frame_id_to_send], start_time);

    log_message("<init>");
}

void Sender::handleMessage(cMessage *msg)
{
    if (auto mes = dynamic_cast<Message_Event*>(msg); mes != nullptr)
    {
        auto ev = mes->getEvent();
        process_event(ev);
        delete mes;
    }
    else if (auto mes = dynamic_cast<Message_Frame*>(msg); mes != nullptr)
    {
        auto frame = mes->getFrame();
        receive_frame(frame);
        delete mes;
    }
}

void
Sender::finish()
{
    log_aggregations();
    out_f.close();
}

void
Sender::schedule_event(Event event, double sch_at)
{
    auto *message = new Message_Event{};
    message->setEvent(event);
    scheduleAt(sch_at, message);
}

void
Sender::process_event(Event &event)
{
    double time_now = (double)simTime().raw() / simTime().getScale();

    // Self timeout
    if (event.error == EVENT_ACK_TIMEOUT)
    {
        auto frame_id = std::stoi(event.content.c_str());
        // No ACK was received
        if (frame_id == frame_id_to_send)
        {
            // Reschedule event
            events[frame_id_to_send].error = ERROR_TYPE_NONE;
            log_message("<timeout> #%d", frame_id_to_send);
            process_event(events[frame_id_to_send]);
        }
        return;
    }

    // Normal message
    num_total_messages++;
    transmission_time = time_now;

    auto error = event.error;
    if (event.error & ERROR_TYPE_LOSS)
    {
        auto frame = make_frame_for_message(event.content.c_str(), time_now);
        log_outbound_frame_with_error(frame, error);

        // Frame is lost
        // Set ACK Timeout
        Event ack_timeout{};
        ack_timeout.error = EVENT_ACK_TIMEOUT;
        ack_timeout.content = std::to_string(frame_id_to_send);
        schedule_event(ack_timeout, time_now + timeout_interval);
        return;
    }

    if (event.error & ERROR_TYPE_DELAY)
    {
        auto frame = make_frame_for_message(event.content.c_str(), time_now);
        log_outbound_frame_before_delay(frame, time_now);

        event.error &= ~((uint8_t) ERROR_TYPE_DELAY);

        // Schedule event with pre-defined delay
        schedule_event(event, time_now + packet_delay);
        return;
    }

    // Make and log outbound frame
    auto frame = make_frame_for_message(event.content.c_str(), time_now);

    if (event.error & ERROR_TYPE_MODIFICATION)
    {
        event.error &= ~((uint8_t) ERROR_TYPE_MODIFICATION);

        // Modify random bit
        const int n_bits = 8 * strlen(frame.payload.c_str());
        int bit_loc = intuniform(0, n_bits - 1);
        int byte_loc = bit_loc / 8;

        char *byte_ptr = (char *)frame.payload.c_str();
        byte_ptr[byte_loc] ^= (char) 1 << (bit_loc - byte_loc * 8);
    }
    if (event.error & ERROR_TYPE_DUPLICATION)
    {
        event.error = EVENT_DUPLICATE_FRAME;

        // Schedule event with 50 ms delay
        schedule_event(event, time_now + 50e-3);
    }

    log_outbound_frame_with_error(frame, error);
    auto *message_frame = new Message_Frame{};
    message_frame->setFrame(frame);
    send(message_frame, "port$o");

    // Set ACK Timeout for normal frames
    if (error != EVENT_DUPLICATE_FRAME)
    {
        Event ack_timeout{};
        ack_timeout.error = EVENT_ACK_TIMEOUT;
        ack_timeout.content = std::to_string(frame_id_to_send);
        schedule_event(ack_timeout, time_now + timeout_interval);
    }
}

void
Sender::receive_frame(Frame frame)
{
    log_inbound_frame(frame);
    num_total_messages++;

    transmission_time = frame.header.sending_time;

    // Check for ack
    auto &header = frame.header;
    if (header.message_type == Message_Type::ACK && header.message_id == frame_id_to_send)
    {
        num_correct_messages++;
        frame_id_to_send++;
        
        // Schedule next event with a 50ms delay
        if (frame_id_to_send < events.size())
            schedule_event(events[frame_id_to_send], header.sending_time + 50e-3);
    }
}

void
Sender::load_input()
{
    std::ifstream input_f(input_filepath);

    for (std::string line; std::getline(input_f, line);)
    {
        auto split_at = line.find(" ");
        auto mod_str = line.substr(0, split_at);
        auto content = line.c_str() + split_at + 1;

        byte modifications = std::stoi(mod_str.c_str(), 0, 2);
        Event ev{};
        ev.error = modifications;
        ev.content = content;
        events.push_back(ev);
    }
}

void
Sender::log_inbound_frame(Frame& frame)
{
    log_message("<received> %s#%d", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id);
}

void
Sender::log_outbound_frame_before_delay(Frame &frame, double originally_at)
{
    log_message("<delayed> %s#%d: %s", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id,
                frame.payload.c_str());
}

void
Sender::log_outbound_duplicate_frame(Frame& frame)
{
    log_message("<duplicate> %s#%d: %s", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id,
                frame.payload.c_str());
}

void
Sender::log_outbound_frame_with_error(Frame& frame, uint8_t error)
{
    log_message("<sending> %s#%d: %s !! %s", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id, 
                frame.payload.c_str(),
                error_type_to_c_str(error));
}

void
Sender::log_aggregations()
{
    log_message("<log_aggregations> Transmission time = %g", transmission_time);
    log_message("<log_aggregations> Total number of transmissions = %d", num_total_messages);

    double throughput = 0.f;
    log_message("<log_aggregations> Throughput = %g", throughput);
}

Frame 
Sender::make_frame_for_message(const char* message, double sch_at)
{
    Frame frame{};

    // Add a header
    frame.header.message_id = frame_id_to_send;
    frame.header.message_type = Message_Type::DATA;
    frame.header.sending_time = sch_at;

    // Byte stuff the message
    std::regex escape{R"((\$|/))"};
    std::string payload = "$";
    payload += std::regex_replace(message, escape, R"(/$1)");
    payload += "$";
    frame.payload = payload;

    // Calculate parity
    frame.trailer = 0;
    for (uint8_t byte: payload)
        frame.trailer ^= byte;

    return frame;
}
