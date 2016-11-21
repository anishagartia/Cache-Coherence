#include "MOESIF_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;
//bool forwarder_present = false;

/*************************
 * Constructor/Destructor.
 *************************/
MOESIF_protocol::MOESIF_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
	// Initialize lines to not have the data yet!
    this->state = MOESIF_CACHE_I;
}

MOESIF_protocol::~MOESIF_protocol ()
{    
}

void MOESIF_protocol::dump (void)
{
    const char *block_states[7] = {"X","I","S","E","O","M","F"};
    fprintf (stderr, "MOESIF_protocol - state: %s\n", block_states[state]);
}

void MOESIF_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
		
	case MOESIF_CACHE_I: do_cache_I (request); break;
	case MOESIF_CACHE_F: do_cache_F (request); break;
    case MOESIF_CACHE_S: do_cache_S (request); break;
	case MOESIF_CACHE_E: do_cache_E (request); break;
    case MOESIF_CACHE_O: do_cache_O (request); break;
    case MOESIF_CACHE_M: do_cache_M (request); break;
	case MOESIF_CACHE_IM: do_cache_IM (request); break;
	case MOESIF_CACHE_IS: do_cache_IS (request); break;
	case MOESIF_CACHE_SM: do_cache_SM (request); break;
	case MOESIF_CACHE_OM: do_cache_OM (request); break;
	case MOESIF_CACHE_FM: do_cache_FM (request); break;
	default:
        fatal_error ("Invalid Cache State for MOESIF Protocol\n");
    }
}

void MOESIF_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
	case MOESIF_CACHE_I: do_snoop_I (request); break;
	case MOESIF_CACHE_F: do_snoop_F (request); break;
    case MOESIF_CACHE_S: do_snoop_S (request); break;
	case MOESIF_CACHE_E: do_snoop_E (request); break;
    case MOESIF_CACHE_O: do_snoop_O (request); break;
    case MOESIF_CACHE_M: do_snoop_M (request); break;
	case MOESIF_CACHE_IM: do_snoop_IM (request); break;
	case MOESIF_CACHE_IS: do_snoop_IS (request); break;
	case MOESIF_CACHE_SM: do_snoop_SM (request); break;
	case MOESIF_CACHE_OM: do_snoop_OM (request); break;
	case MOESIF_CACHE_FM: do_snoop_FM (request); break;
    default:
    	fatal_error ("Invalid Cache State for MOESIF Protocol\n");
    }
}

// *********** do_cache_F ************//
inline void MOESIF_protocol::do_cache_F (Mreq *request)
{
	//fprintf(stderr,"here c f");
	switch (request->msg) {
    /* The S state means we have the data it is clean (according to itself, although system may have an Owner), and we cannot modify it.  * Therefore a request from the processor can be immediately satisfied only if it is a read (load). 
	* In S state, and if the processor requests to write/modify the data, it must go to M state. Thus we must send GetM on the bus to inform other processors of this change.
     */
    case LOAD:		
		send_DATA_to_proc(request->addr);		
    	break;
    case STORE:					
		send_GETM(request->addr);	
		state = MOESIF_CACHE_FM;	
		// Choherence miss
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }

}
// *********** do_cache_I ************//
inline void MOESIF_protocol::do_cache_I (Mreq *request)
{
	//fprintf(stderr,"here c i");
	switch (request->msg) {
    /** If we get a request from the processor we need to get the data. 
	* if it is a load request, it is for reading the data. we must send GetS
	* if it is a store request, the processor wants to write to the block, and jus modify it. We must send GetM
	**/
    case LOAD:
		send_GETS(request->addr);		
		state = MOESIF_CACHE_IS;
		Sim->cache_misses++;		
		break;
    case STORE: 
		send_GETM(request->addr);
		state = MOESIF_CACHE_IM;
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// *********** do_cache_S ************//
inline void MOESIF_protocol::do_cache_S (Mreq *request)
{
	//fprintf(stderr,"here c s");
	switch (request->msg) {
    /* The S state means we have the data it is clean (according to itself, although system may have an Owner), and we cannot modify it.  * Therefore a request from the processor can be immediately satisfied only if it is a read (load). 
	* In S state, and if the processor requests to write/modify the data, it must go to M state. Thus we must send GetM on the bus to inform other processors of this change.
     */
    case LOAD:		
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:					
		send_GETM(request->addr);	
		state = MOESIF_CACHE_SM;	
		// Choherence miss
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// *********** do_cache_E ************//
inline void MOESIF_protocol::do_cache_E (Mreq *request)
{
	//fprintf(stderr,"here c e");
	switch (request->msg) {
    /* The S state means we have the data it is clean (according to itself, although system may have an Owner), and we cannot modify it.  * Therefore a request from the processor can be immediately satisfied only if it is a read (load). 
	* In S state, and if the processor requests to write/modify the data, it must go to M state. Thus we must send GetM on the bus to inform other processors of this change.
     */
    case LOAD:		
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:							
		send_DATA_to_proc(request->addr);
		state = MOESIF_CACHE_M;	
		// Choherence miss
		Sim->silent_upgrades++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// *********** do_cache_O ************//
inline void MOESIF_protocol::do_cache_O (Mreq *request)
{
	//fprintf(stderr,"here c o");
	switch (request->msg) {
    case LOAD:				
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:    
		//set_shared_line();
		state = MOESIF_CACHE_OM;
		send_GETM(request->addr);
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// *********** do_cache_M ************//
inline void MOESIF_protocol::do_cache_M (Mreq *request)
{
	//fprintf(stderr,"here c m");
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
// *********** do_cache_IM ************//
inline void MOESIF_protocol::do_cache_IM (Mreq *request)
{
	//fprintf(stderr,"here c im");
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
// *********** do_cache_IS ************//
inline void MOESIF_protocol::do_cache_IS (Mreq *request)
{
	//fprintf(stderr,"here c is");
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
// *********** do_cache_SM ************//
inline void MOESIF_protocol::do_cache_SM (Mreq *request)
{
	//fprintf(stderr,"here c sm");
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
// *********** do_cache_OM ************//
inline void MOESIF_protocol::do_cache_OM (Mreq *request)
{
	//fprintf(stderr,"here c om");
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

// *********** do_cache_FM ************//
inline void MOESIF_protocol::do_cache_FM (Mreq *request)
{
	//fprintf(stderr,"here c fm");
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


// ########## Snoop Protocols ##########

// ########## do_snoop_F ##########//
inline void MOESIF_protocol::do_snoop_F (Mreq *request)
{
	//fprintf(stderr,"here s f");
	switch (request->msg) {
    case GETS:
		send_DATA_on_bus(request->addr,request->src_mid);		
		set_shared_line();
		//forwarder_present = true;
		break;
    case GETM:		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_I;		
		break;
    case DATA:				
		set_shared_line();
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_I ##########//
inline void MOESIF_protocol::do_snoop_I (Mreq *request)
{
	//fprintf(stderr,"here s i");
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
// ########## do_snoop_S ##########//
inline void MOESIF_protocol::do_snoop_S (Mreq *request)
{
	//fprintf(stderr,"here s s");
	switch (request->msg) {
    case GETS:
		set_shared_line();
		/* if (forwarder_present == false){
			state = MOESIF_CACHE_F;
			fprintf(stderr,"**New state is f");
			send_DATA_to_proc(request->addr);		
			forwarder_present = true;
		} */		
		break;
    case GETM:	
		state = MOESIF_CACHE_I;						
		break;
    case DATA:		
		set_shared_line();
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_E ##########//
inline void MOESIF_protocol::do_snoop_E (Mreq *request)
{
	//fprintf(stderr,"here s e");
	switch (request->msg) {
    case GETS:
		/** In E state, this cache is the only cache which has the block. 
		* If it sees a get S, another cache is requesting it, thus change to F state		
		**/		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_F;		
		set_shared_line();		
		break;
    case GETM:
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_I;
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
// ########## do_snoop_O ##########//
inline void MOESIF_protocol::do_snoop_O (Mreq *request)
{	
	//fprintf(stderr,"here s o");
	switch (request->msg) {
    case GETS:
		//forwarder_present = true;
		set_shared_line();
		send_DATA_on_bus(request->addr,request->src_mid);				
		break;
    case GETM:		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_I;		
		break;
    case DATA:
		set_shared_line();
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_M ##########//
inline void MOESIF_protocol::do_snoop_M (Mreq *request)
{
	//fprintf(stderr,"here s m");
	switch (request->msg) {
    case GETS:
		state = MOESIF_CACHE_O;
		set_shared_line();
		send_DATA_on_bus(request->addr,request->src_mid);				
		break;
    case GETM:		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_I;		
		break;
    case DATA:
		request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_IM ##########//
inline void MOESIF_protocol::do_snoop_IM (Mreq *request)
{
	//fprintf(stderr,"here s im");
	switch (request->msg) {
    case GETS:
    case GETM:
		break;
    case DATA:
		send_DATA_to_proc(request->addr);
		state = MOESIF_CACHE_M;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_IS ##########//
inline void MOESIF_protocol::do_snoop_IS (Mreq *request)
{
	//fprintf(stderr,"here s is");
	switch (request->msg) {
    case GETS:
    case GETM:
		break;
    case DATA:
		send_DATA_to_proc(request->addr);
		if (get_shared_line()){
			state = MOESIF_CACHE_S;						
		}
		else {
			state = MOESIF_CACHE_E;								
			//fprintf(stderr,"**New state is e");
		}			
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_SM ##########//
inline void MOESIF_protocol::do_snoop_SM (Mreq *request)
{
	//fprintf(stderr,"here s sm");
	switch (request->msg) {
    case GETS:
		/* if (//forwarder_present == false){
			send_DATA_on_bus(request->addr,request->src_mid);		
			set_shared_line();
			//forwarder_present = true;
			state = MOESIF_CACHE_FM;
			fprintf(stderr,"**New state is fm");
		} */
		set_shared_line();
		break;
    case GETM:		
		//state = MOESIF_CACHE_I;		
		break;
    case DATA:				
		state = MOESIF_CACHE_M;
		send_DATA_to_proc(request->addr);		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

// ########## do_snoop_OM ##########//
inline void MOESIF_protocol::do_snoop_OM (Mreq *request)
{
	//fprintf(stderr,"here s om");
	switch (request->msg) {
    case GETS:
		//forwarder_present = true;
		send_DATA_on_bus(request->addr,request->src_mid);		
		set_shared_line();
		break;
    case GETM:						
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_IM;		
		break;
    case DATA:				
		state = MOESIF_CACHE_M;
		send_DATA_to_proc(request->addr);		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}
// ########## do_snoop_FM ##########//
inline void MOESIF_protocol::do_snoop_FM (Mreq *request)
{
	//fprintf(stderr,"here s fm");
	switch (request->msg) {
    case GETS:
		send_DATA_on_bus(request->addr,request->src_mid);		
		set_shared_line();
		//forwarder_present = true;
		break;
    case GETM:						
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MOESIF_CACHE_IM;		
		break;
    case DATA:				
		state = MOESIF_CACHE_M;
		send_DATA_to_proc(request->addr);		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}







