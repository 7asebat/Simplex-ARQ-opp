package src;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    parameters:
        string output_filepath = default("output0.txt");

    submodules:
        sender: Sender {
            @display("p=61,69");
        }
        receiver: Receiver {
            @display("p=174,69");
        }
    connections:
        sender.port <--> receiver.port;
}

