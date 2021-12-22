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
#include <fstream>

#include <ARQ.h>
#include <Message_m.h>

/**
 * TODO - Generated class
 */
class Receiver : public cSimpleModule
{
    uint8_t frame_id_to_receive;
    std::ofstream out_f;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    uint8_t
    frame_calculate_parity(Frame);

    void
    receive_frame(Frame);

    void
    log_inbound_frame(Frame);

    void
    log_outbound_frame(Frame);

    template <typename ...Args>
    inline void
    log_message(const char* fmt, Args&& ...args)
    {
        double time_now = (double)simTime().raw() / simTime().getScale();
        char buffer[1024];
        sprintf(buffer, "@%g[Receiver] ", time_now);
        sprintf(buffer + strlen(buffer), fmt, args...);
        sprintf(buffer + strlen(buffer), "\n");

        EV_INFO << buffer;
        out_f << buffer;
    }
};
