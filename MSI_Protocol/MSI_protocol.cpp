#include "MSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MSI_protocol::MSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
}

MSI_protocol::~MSI_protocol ()
{    
}

void MSI_protocol::dump (void)
{
    const char *block_states[4] = {"X","I","S","M"};
    fprintf (stderr, "MSI_protocol - state: %s\n", block_states[state]);
}

void MSI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {

    default:
        fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

void MSI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {

    default:
    	fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

inline void MSI_protocol::do_cache_I (Mreq *request)
{

}

inline void MSI_protocol::do_cache_S (Mreq *request)
{

}

inline void MSI_protocol::do_cache_M (Mreq *request)
{

}

inline void MSI_protocol::do_snoop_I (Mreq *request)
{

}

inline void MSI_protocol::do_snoop_S (Mreq *request)
{

}

inline void MSI_protocol::do_snoop_M (Mreq *request)
{

}
