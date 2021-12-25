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

	use_hamming = getParentModule()->par("use_hamming").boolValue();

	auto output_filepath = getParentModule()->par("output_filepath").stringValue();
	out_f.open(output_filepath, out_f.out | out_f.app);

	load_input();
	schedule_event(events[frame_id_to_send], start_time);

	log_message("<init> Errors are handled by (%s)", use_hamming ? "Hamming Code" : "Parity Byte");
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
    else if (auto mes = dynamic_cast<Message_Duplicate_Frame*>(msg); mes != nullptr)
    {
        auto frame = mes->getFrame();
        log_outbound_duplicate_frame(frame);
        num_total_messages++;

        // Forward duplicate frame
        if (mes->getLost() == false)
        {
            auto *duplicate = new Message_Frame{};
            duplicate->setFrame(frame);
            send(duplicate, "port$o");
        }
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
Sender::schedule_event(const Event &event, double sch_at)
{
    auto *message = new Message_Event{};
    message->setEvent(event);
    scheduleAt(sch_at, message);
}

void
Sender::process_event(const Event &event)
{
    double time_now = (double)simTime().raw() / simTime().getScale();

    // Self timeout
    if (event.error == EVENT_ACK_TIMEOUT)
    {
        // No ACK was received
        if (event.message_id == frame_id_to_send)
        {
            // Reschedule event with no errors
            events[frame_id_to_send].error = ERROR_TYPE_NONE;
            log_message("<timeout> #%d", frame_id_to_send);
            process_event(events[frame_id_to_send]);
        }
        return;
    }

	// Send and log message, Update transmission time
	auto frame = make_frame_for_message(event, time_now);

	if (event.error & ERROR_TYPE_DELAY)
	{
		log_delayed_frame(frame, time_now);

		// Schedule event with pre-defined delay
		// Keep other modifications
		Event delayed = event;
		delayed.error ^= ERROR_TYPE_DELAY;
		schedule_event(delayed, time_now + packet_delay);
		return;
	}
	if (event.error & ERROR_TYPE_MODIFICATION)
	{
		modify_random_bit(frame.payload);
	}
	if (event.error & ERROR_TYPE_DUPLICATION)
	{
		// Schedule duplicate frame
		// after a short delay (0.01s)
		auto *message = new Message_Duplicate_Frame{};
		message->setFrame(frame);
		message->setLost(event.error & ERROR_TYPE_LOSS);
		scheduleAt(time_now + 10e-3, message);
	}

	log_outbound_frame_with_error(frame, event.error);
	num_total_messages++;
	transmission_time = time_now;

	// Send only if the frame isn't lost
	if ((event.error & ERROR_TYPE_LOSS) == 0)
	{
		auto *message = new Message_Frame{};
		message->setFrame(frame);
		send(message, "port$o");
	}

	// Set ACK Timeout for normal frames
	Event ack_timeout{};
	ack_timeout.error	   = EVENT_ACK_TIMEOUT;
	ack_timeout.message_id = event.message_id;
	schedule_event(ack_timeout, time_now + timeout_interval);
}

void
Sender::modify_random_bit(Payload &payload)
{
	int bit_to_flip;
	if (use_hamming)
		bit_to_flip = intuniform(0, Hamming::N_CODEWORD - 1);
	else
		bit_to_flip = intuniform(0, Hamming::N_DATA - 1);

	int byte_to_flip = intuniform(0, payload.size() - 1);

	payload[byte_to_flip][bit_to_flip] = !payload[byte_to_flip][bit_to_flip];
}

void
Sender::receive_frame(const Frame &frame)
{
    log_inbound_frame(frame);
    num_total_messages++;
    transmission_time = frame.header.sending_time;

    // Check for ack
    auto &header = frame.header;
    if (header.message_id == frame_id_to_send)
    {
        if (header.message_type == Message_Type::ACK)
        {
            num_correct_messages++;
            frame_id_to_send++;
        }
        // Send clean frame
        else if (header.message_type == Message_Type::NACK)
        {
            events[frame_id_to_send].error = ERROR_TYPE_NONE;
        }

        // Schedule next event right away
        if (frame_id_to_send < events.size())
            schedule_event(events[frame_id_to_send], header.sending_time);
    }
}

void
Sender::load_input()
{
    std::ifstream input_f(input_filepath);
    uint8_t id = 0;

    for (std::string line; std::getline(input_f, line); id++)
    {
        auto split_at = line.find(" ");
        auto mod_str = line.substr(0, split_at);
        auto content = line.c_str() + split_at + 1;

        byte modifications = std::stoi(mod_str.c_str(), 0, 2);
        Event ev{};
        ev.error = modifications;
        ev.content = content;
        ev.message_id = id;
        events.push_back(ev);
    }
}

void
Sender::log_inbound_frame(const Frame& frame)
{
    log_message("<received> %s#%d", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id);
}

void
Sender::log_delayed_frame(const Frame &frame, double originally_at)
{
	auto content = use_hamming ? Hamming::decode_payload(frame.payload) : payload_to_string(frame.payload);
	log_message("<delayed> %s#%d: %s",
				message_type_to_c_str(frame.header.message_type),
				frame.header.message_id,
				content.c_str());
}

void
Sender::log_outbound_duplicate_frame(const Frame& frame)
{
	auto content = use_hamming ? Hamming::decode_payload(frame.payload) : payload_to_string(frame.payload);
	log_message("<duplicate> %s#%d: %s",
				message_type_to_c_str(frame.header.message_type),
				frame.header.message_id,
				content.c_str());
}

void
Sender::log_outbound_frame_with_error(const Frame& frame, uint8_t error)
{
	auto content = use_hamming ? Hamming::decode_payload(frame.payload) : payload_to_string(frame.payload);
	log_message("<sending> %s#%d: %s !! %s",
				message_type_to_c_str(frame.header.message_type),
				frame.header.message_id,
				content.c_str(),
				error_type_to_c_str(error));
}

void
Sender::log_aggregations()
{
    transmission_time -= start_time;
    log_message("<log_aggregations> Transmission time = %g", transmission_time);
    log_message("<log_aggregations> Total number of transmissions = %d", num_total_messages);

    double throughput = num_correct_messages / transmission_time;
    log_message("<log_aggregations> Throughput = %g", throughput);
}

Frame 
Sender::make_frame_for_message(const Event &event, double sch_at)
{
    Frame frame{};

    // Add a header
    frame.header.message_id = event.message_id;
    frame.header.message_type = Message_Type::DATA;
    frame.header.sending_time = sch_at;

    // Byte stuff the message
    std::regex escape{R"((\$|/))"};
    std::string payload = "$";
    payload += std::regex_replace(event.content.c_str(), escape, R"(/$1)");
    payload += "$";

	if (use_hamming)
		frame.payload = Hamming::encode_payload(payload);
	else
	{
		for (auto byte : payload)
			frame.payload.emplace_back(byte);
	}

	// Calculate parity
	frame.trailer = 0;
	for (uint8_t byte : payload)
		frame.trailer ^= byte;

	return frame;
}
