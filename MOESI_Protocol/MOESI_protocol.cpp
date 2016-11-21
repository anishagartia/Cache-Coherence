#include "MOESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOESI_protocol::MOESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
}

MOESI_protocol::~MOESI_protocol ()
{    
}

void MOESI_protocol::dump (void)
{
    const char *block_states[6] = {"X","I","S","E","O","M"};
    fprintf (stderr, "MOESI_protocol - state: %s\n", block_states[state]);
}

void MOESI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {

    default:
        fatal_error ("Invalid Cache State for MOESI Protocol\n");
    }
}

void MOESI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {

    default:
    	fatal_error ("Invalid Cache State for MOESI Protocol\n");
    }
}

inline void MOESI_protocol::do_cache_I (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_S (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_E (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_O (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_M (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_I (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_S (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_E (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_O (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_M (Mreq *request)
{

}


