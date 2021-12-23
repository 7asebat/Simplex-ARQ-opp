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
#pragma once
#include <vector>
#include <fstream>

#include <ARQ.h>
#include <Message_m.h>

/**
 * TODO - Generated class
 */
class Sender : public cSimpleModule
{
protected:
    int num_correct_messages;
    int num_total_messages;
    double transmission_time;

    // Parameters
    double start_time;
    double timeout_interval;
    double packet_delay;
    std::string input_filepath;

    // Output file
    std::ofstream out_f;

    // ARQ
    uint8_t frame_id_to_send;
    std::vector<Event> events;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    void
    load_input();

    // SCHEDULE
    void
    schedule_event(const Event&, double sch_at);

    // RECEIVE
    void
    process_event(const Event&);

    void
    receive_frame(const Frame&);

    // Utility
    Frame 
    make_frame_for_message(const Event&, double sch_at);

    // LOG
    void
    log_inbound_frame(const Frame&);

    void
    log_outbound_frame_before_delay(const Frame&, double originally_at);

    void
    log_outbound_duplicate_frame(const Frame&);

    void
    log_outbound_frame_with_error(const Frame&, uint8_t error);

    void
    log_aggregations();

    template <typename ...Args>
    inline void
    log_message(const char* fmt, Args&& ...args)
    {
        double time_now = (double)simTime().raw() / simTime().getScale();
        char buffer[1024];
        sprintf(buffer, "@%g [Sender] ", time_now);
        sprintf(buffer + strlen(buffer), fmt, args...);
        sprintf(buffer + strlen(buffer), "\n");

        EV_INFO << buffer;
        out_f << buffer;
    }

};
