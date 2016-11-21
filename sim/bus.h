#ifndef BUS_H_
#define BUS_H_

#include "types.h"

class Mreq;

class Bus{
public:
    Bus();
    ~Bus();

    //TODO: Add shared, flush lines, etc...

	Mreq *current_request;
    LIST <Mreq *>pending_requests;
    Mreq *data_reply;
    
    bool request_in_progress;

    bool shared_line;

    void tick ();

    bool is_shared_active () { return shared_line; }
    bool bus_request (Mreq * request);
    Mreq *bus_snoop();
};

#endif
