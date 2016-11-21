#include "MOSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOSI_protocol::MOSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
	// Initialize lines to not have the data yet!
    this->state = MOSI_CACHE_I;
}

MOSI_protocol::~MOSI_protocol ()
{    
}

void MOSI_protocol::dump (void)
{
    const char *block_states[5] = {"X","I","S","O","M"};
    fprintf (stderr, "MOSI_protocol - state: %s\n", block_states[state]);
}

void MOSI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
		
	case MOSI_CACHE_I: do_cache_I (request); break;
    case MOSI_CACHE_S: do_cache_S (request); break;
    case MOSI_CACHE_O: do_cache_O (request); break;
    case MOSI_CACHE_M: do_cache_M (request); break;
	case MOSI_CACHE_IM: do_cache_IM (request); break;
	case MOSI_CACHE_IS: do_cache_IS (request); break;
	case MOSI_CACHE_SM: do_cache_SM (request); break;
	case MOSI_CACHE_OStr: do_cache_OStr (request); break;
	default:
        fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

void MOSI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
	case MOSI_CACHE_I: do_snoop_I (request); break;
    case MOSI_CACHE_S: do_snoop_S (request); break;
    case MOSI_CACHE_O: do_snoop_O (request); break;
    case MOSI_CACHE_M: do_snoop_M (request); break;
	case MOSI_CACHE_IM: do_snoop_IM (request); break;
	case MOSI_CACHE_IS: do_snoop_IS (request); break;
	case MOSI_CACHE_SM: do_snoop_SM (request); break;
	case MOSI_CACHE_OStr: do_snoop_OStr (request); break;
    default:
    	fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

// *********** do_cache_I ************//
inline void MOSI_protocol::do_cache_I (Mreq *request)
{

	switch (request->msg) {
    /** If we get a request from the processor we need to get the data. 
	* if it is a load request, it is for reading the data. we must send GetS
	* if it is a store request, the processor wants to write to the block, and jus modify it. We must send GetM
	**/
    case LOAD:
		send_GETS(request->addr);		
		state = MOSI_CACHE_IS;
		Sim->cache_misses++;		
		break;
    case STORE: 
		send_GETM(request->addr);
		state = MOSI_CACHE_IM;
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }

}

// *********** do_cache_IS ************//
inline void MOSI_protocol::do_cache_IS (Mreq *request)
{
	switch (request->msg) {
    /* If the block is in the IM state that means it sent out a GETS message
	 * and is waiting on DATA.  Therefore the processor should be waiting
	 * on a pending request. Therefore we should not be getting any requests from
	 * the processor.
	 */	 
    case LOAD:
    case STORE:
		request->print_msg (my_table->moduleID, "ERROR");
		fatal_error("Should only have one outstanding request per processor!");
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// *********** do_cache_IM ************//
inline void MOSI_protocol::do_cache_IM (Mreq *request)
{
	switch (request->msg) {
    /* If the block is in the IM state that means it sent out a GETM message
	 * and is waiting on DATA.  Therefore the processor should be waiting
	 * on a pending request. Therefore we should not be getting any requests from
	 * the processor.
	 */
    case LOAD:
    case STORE:  
		request->print_msg (my_table->moduleID, "ERROR");
		fatal_error("Should only have one outstanding request per processor!");
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }

}

// *********** do_cache_S ************//
inline void MOSI_protocol::do_cache_S (Mreq *request)
{
	switch (request->msg) {
    /* The S state means we have the data it is clean (according to itself, although system may have an Owner), and we cannot modify it.  * Therefore a request from the processor can be immediately satisfied only if it is a read (load). 
	* In S state, and if the processor requests to write/modify the data, it must go to M state. Thus we must send GetM on the bus to inform other processors of this change.
     */
    case LOAD:		
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:					
		send_GETM(request->addr);	
		state = MOSI_CACHE_SM;	
		// Choherence miss
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// *********** do_cache_SM ************//
inline void MOSI_protocol::do_cache_SM (Mreq *request)
{
	switch (request->msg) {
    case LOAD:		
    case STORE:    
		request->print_msg (my_table->moduleID, "ERROR");
		fatal_error("Should only have one outstanding request per processor!");
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// *********** do_cache_O ************//
inline void MOSI_protocol::do_cache_O (Mreq *request)
{		
	switch (request->msg) {
    case LOAD:		
		//set_shared_line();
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:    
		//set_shared_line();
		state = MOSI_CACHE_OStr;
		send_GETM(request->addr);
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// *********** do_cache_OStr ************//
inline void MOSI_protocol::do_cache_OStr (Mreq *request)
{
	switch (request->msg) {
    case LOAD:				
    	break;
    case STORE:    		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// *********** do_cache_M ************//
inline void MOSI_protocol::do_cache_M (Mreq *request)
{
	switch (request->msg) {
    case LOAD:		
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:    
		send_DATA_to_proc(request->addr);
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## Snoop Protocols ##########

// ########## do_snoop_I ##########//
inline void MOSI_protocol::do_snoop_I (Mreq *request)
{
	switch (request->msg) {
    case GETS:
    case GETM:
    case DATA:
    	/**
    	 * If we snoop a message from another cache and we are in I, then we don't
    	 * need to do anything!  We obviously cannot supply data since we don't have
    	 * it, and we don't need to downgrade our state since we are already in I.
    	 */
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_IS ##########//
inline void MOSI_protocol::do_snoop_IS (Mreq *request)
{
	switch (request->msg) {
    case GETS:
    case GETM:
		break;
    case DATA:
		send_DATA_to_proc(request->addr);
		state = MOSI_CACHE_S;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_IM ##########//
inline void MOSI_protocol::do_snoop_IM (Mreq *request)
{
	switch (request->msg) {
    case GETS:
    case GETM:
		break;
    case DATA:
		send_DATA_to_proc(request->addr);
		state = MOSI_CACHE_M;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_S ##########
inline void MOSI_protocol::do_snoop_S (Mreq *request)
{
	switch (request->msg) {
    case GETS:
		set_shared_line();
		break;
    case GETM:	
		state = MOSI_CACHE_I;						
		break;
    case DATA:		
		set_shared_line();
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## do_snoop_SM ##########
inline void MOSI_protocol::do_snoop_SM (Mreq *request)
{
	switch (request->msg) {
    case GETS:
		set_shared_line();
		break;
    case GETM:		
		state = MOSI_CACHE_IM;		
		break;
    case DATA:				
		state = MOSI_CACHE_M;
		send_DATA_to_proc(request->addr);		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## do_snoop_O ##########
inline void MOSI_protocol::do_snoop_O (Mreq *request)
{	
	switch (request->msg) {
    case GETS:
		set_shared_line();
		send_DATA_on_bus(request->addr,request->src_mid);				
		break;
    case GETM:		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOSI_CACHE_I;		
		break;
    case DATA:
		request->print_msg (my_table->moduleID, "ERROR");
		fatal_error("Should only have one outstanding request per processor!");
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## do_snoop_OStr ##########
inline void MOSI_protocol::do_snoop_OStr (Mreq *request)
{
	switch (request->msg) {
    case GETS:
		send_DATA_on_bus(request->addr,request->src_mid);		
		set_shared_line();
		break;
    case GETM:				
		set_shared_line();
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOSI_CACHE_IM;		
		break;
    case DATA:				
		state = MOSI_CACHE_M;
		send_DATA_to_proc(request->addr);		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## do_snoop_M ##########
inline void MOSI_protocol::do_snoop_M (Mreq *request)
{
	switch (request->msg) {
    case GETS:
		state = MOSI_CACHE_O;
		//set_shared_line();
		send_DATA_on_bus(request->addr,request->src_mid);				
		break;
    case GETM:
		Sim->bus->shared_line = false;
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOSI_CACHE_I;		
		break;
    case DATA:
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

