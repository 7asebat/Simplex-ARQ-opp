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

#include "Receiver.h"

Define_Module(Receiver);

void Receiver::initialize()
{
    frame_id_to_receive = 0;

    auto output_filepath = getParentModule()->par("output_filepath").stringValue();
    out_f.open(output_filepath, out_f.out | out_f.app);
    log_message("<init>");
}

void Receiver::handleMessage(cMessage *msg)
{
    if (auto mes = dynamic_cast<Message_Frame*>(msg); mes != nullptr)
    {
        auto frame = mes->getFrame();

        // Forward scheduled ack
        if (mes->isSelfMessage())
        {
            log_outbound_frame(frame);
            send(mes, "port$o");
        }
        else
        {
            receive_frame(frame);
            delete mes;
        }
    }
}

void Receiver::finish()
{
    log_message("<finish>");
    out_f.close();
}

uint8_t 
Receiver::frame_calculate_parity(const Frame &frame)
{
    uint8_t parity = frame.trailer;
    std::string payload = frame.payload.c_str();

    for (uint8_t byte : payload)
        parity ^= byte;

    return parity;
}

void 
Receiver::receive_frame(const Frame &frame)
{
    double time_now = (double)simTime().raw() / simTime().getScale();
    log_inbound_frame(frame);

    Frame response{};
    response.header.sending_time = time_now + 0.2;
    response.header.message_id = frame.header.message_id;

    // No errors
    if (frame_calculate_parity(frame) == 0)
    {
        response.header.message_type = Message_Type::ACK;

        // Received the right frame
        if (frame.header.message_id == frame_id_to_receive)
            frame_id_to_receive++;
        else
            log_message("<discarding> Wrong frame");
    }
    else
    {
        response.header.message_type = Message_Type::NACK;
        log_message("<discarding> Parity check failed");
    }

    // Schedule response after 0.2s
    auto *message = new Message_Frame{};
    message->setFrame(response);
    scheduleAt(response.header.sending_time, message);
}

void
Receiver::log_inbound_frame(const Frame &frame)
{
    log_message("<received> %s#%d: %s", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id,
                frame.payload.c_str());
}

void
Receiver::log_outbound_frame(const Frame &frame)
{
    log_message("<sending> %s#%d", 
                message_type_to_c_str(frame.header.message_type), 
                frame.header.message_id);
}
