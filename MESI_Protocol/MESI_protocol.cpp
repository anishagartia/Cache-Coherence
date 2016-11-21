#include "MESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MESI_protocol::MESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
	// Initialize lines to not have the data yet!
    this->state = MESI_CACHE_I;
}

MESI_protocol::~MESI_protocol ()
{    

}

void MESI_protocol::dump (void)
{
    const char *block_states[5] = {"X","I","S","E","M"};
    fprintf (stderr, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
	case MESI_CACHE_I:  do_cache_I (request); break;
    case MESI_CACHE_IM: do_cache_IM (request); break;
	case MESI_CACHE_IS: do_cache_IS (request); break;
    case MESI_CACHE_M:  do_cache_M (request); break;
	case MESI_CACHE_E:  do_cache_E (request); break;    
    case MESI_CACHE_S:  do_cache_S (request); break;
	case MESI_CACHE_SM:  do_cache_SM (request); break;
    default:
        fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

void MESI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
	case MESI_CACHE_I:  do_snoop_I (request); break;
    case MESI_CACHE_IM: do_snoop_IM (request); break;
	case MESI_CACHE_IS: do_snoop_IS (request); break;
    case MESI_CACHE_M:  do_snoop_M (request); break;
	case MESI_CACHE_E:  do_snoop_E (request); break;    
    case MESI_CACHE_S:  do_snoop_S (request); break;
	case MESI_CACHE_SM:  do_snoop_SM (request); break;

    default:
    	fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

inline void MESI_protocol::do_cache_I (Mreq *request)
{
	//fprintf(stderr,"here c i");
	switch (request->msg) {
    /** If we get a request from the processor we need to get the data. 
	* if it is a load request, it is for reading the data. we must send GetS
	* if it is a store request, the processor wants to write to the block, and jus modify it. We must send GetM
	**/
    case LOAD:
		send_GETS(request->addr);		
		state = MESI_CACHE_IS;
		Sim->cache_misses++;		
		break;
    case STORE: 
		send_GETM(request->addr);
		state = MESI_CACHE_IM;
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_IM (Mreq *request)
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

inline void MESI_protocol::do_cache_IS (Mreq *request)
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

inline void MESI_protocol::do_cache_S (Mreq *request)
{	
	//fprintf(stderr,"here c s");
	switch (request->msg) {
    /* The S state means we have the data it is clean, and we cannot modify it.  * Therefore a request from the processor can be immediately satisfied only if it is a read (load). 
	* In S state, and if the processor requests to write/modify the data, it must go to M state. Thus we must send GetM on the bus to inform other processors of this change.
     */
    case LOAD:
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:					
		send_GETM(request->addr);	
		//send_DATA_to_proc(request->addr);		
		//send_DATA_on_bus(request->addr,request->dest_mid);		
		state = MESI_CACHE_SM;	
		// Choherence miss
		Sim->cache_misses++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_SM (Mreq *request)
{
	//fprintf(stderr,"here c sm");
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

inline void MESI_protocol::do_cache_E (Mreq *request)
{
	//fprintf(stderr,"here c e");
	switch (request->msg) {
    /** in E state, the cache is the only one with they block. It is clean, and no other cache has it.
	* In E state if the processor requests for the block for read, it is available and thus a hit, and sends it to the PE.
	* If the processor request for the block for write/modify, since only this PE has it, it can silently update to M state without a GetM.
	**/
    case LOAD:
		send_DATA_to_proc(request->addr);
    	break;
    case STORE:
		// Hit, and Silent update to M state.
		send_DATA_to_proc(request->addr);
		state = MESI_CACHE_M;
		Sim->silent_upgrades++;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_M (Mreq *request)
{
	//fprintf(stderr,"here c m");
	switch (request->msg) {
    /** In M state, this cache is the only one who has a copy of the block, but it is dirty.
	* Load- Supply the block to processor as it is most recent. It is a hit.
	* Store- Supply the block to processor as it is most recent, not required to inform other PE since it is only PR with it. It is a hit.
	**/
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
 // Snoop protocols
 
inline void MESI_protocol::do_snoop_I (Mreq *request)
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

inline void MESI_protocol::do_snoop_IM (Mreq *request)
{
	//fprintf(stderr,"here s im");
    switch (request->msg) {
    case GETS:
    case GETM:
		/** While in IM we will see our own GETS or GETM on the bus.  We should just ignore it and wait for DATA to show up.
		* No other GetM/GetS can be there on bus since we have sent one.
		 */
		 break;
    case DATA:
    	/** IM state meant that the block had sent the GET and was waiting on DATA.		
    	 * If we snoop a message from another cache and we are in M, then it is the data we requested for. Thus we should copy it and give to processor.
    	 */		 
		send_DATA_to_proc(request->addr);
		if (get_shared_line())
		{
			Sim->bus->shared_line = false;
		}		
		state = MESI_CACHE_M;			
		break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IS (Mreq *request)
{
	//fprintf(stderr,"here s is");
    switch (request->msg) {
		/** IM state meant that the block had sent the GET and was waiting on DATA.		
    	 * If we snoop a message from another cache and we are in S, then it is the data we requested for. Thus we should copy it and give to processor.
    	 */
    case GETS:
    case GETM:
		break;
    case DATA:
		send_DATA_to_proc(request->addr);
		if (get_shared_line()){
			state = MESI_CACHE_S;			
			//fprintf(stderr,"**New state s is, S %d",state);
		}
		else {
			state = MESI_CACHE_E;					
			//fprintf(stderr,"**New state s is, E %d",state);
		}		
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_S (Mreq *request)
{
	//fprintf(stderr,"here s s");
    switch (request->msg) {
    case GETS:		
		/*Mreq *temp;
		temp = new Mreq ();
		temp = Sim->bus->bus_snoop();
		if ( temp->msg == DATA ){
			break;
		}
		else {
			send_DATA_on_bus(request->addr,request->src_mid);						
			break;
		}*/
		//send_DATA_on_bus(request->addr,request->src_mid);						
		// shared line is reset every cycle. Thus must set to shared again.
		set_shared_line(); 
		break;
		
    case GETM:	
		// Another cache is requesting the block to modify it. Thus the value held in shared block will be stale. Thus change it to I.
		//send_DATA_on_bus(request->addr,request->src_mid);
		state = MESI_CACHE_I;
		if (get_shared_line())
		{
			//Sim->bus->shared_line = false;
		}				
		break;
    case DATA:
    	/** In S state, line is shared. Thus if same data is available on line, doesnot make a difference to state of this PE. Just setting shared bit.
    	*/		
		set_shared_line();
		break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: S state shouldn't see this message\n");
    }
}


inline void MESI_protocol::do_snoop_SM (Mreq *request)
{
	//set_shared_line();
	//fprintf(stderr,"here s sm");
    switch (request->msg) {
    case GETS:	
		set_shared_line();
		break;
    case GETM:	
		//send_DATA_on_bus(request->addr,request->dest_mid);
		//send_DATA_to_proc(request->addr);
		//state = MESI_CACHE_M;
		//Sim->bus->shared_line = false;
		//set_shared_line();
		break;
    case DATA:		
		//send_DATA_on_bus(request->addr,request->src_mid);
		send_DATA_to_proc(request->addr);
		state = MESI_CACHE_M;
		Sim->bus->shared_line = false;
    	break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: SM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_E (Mreq *request)
{
	//fprintf(stderr,"here s e");
    switch (request->msg) {
    case GETS:
		/** In E state, this cache is the only cache which has the block. 
		* If it sees a get S, another cache is requesting it, thus change to S state		
		**/		
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MESI_CACHE_S;		
		set_shared_line();		
		break;
    case GETM:
		send_DATA_on_bus(request->addr,request->src_mid);		
		state = MESI_CACHE_I;
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

inline void MESI_protocol::do_snoop_M (Mreq *request)
{
	//fprintf(stderr,"here s m");
    switch (request->msg) {
    case GETS:
		send_DATA_on_bus(request->addr,request->src_mid);
		state = MESI_CACHE_S;
		set_shared_line();
		break;
    case GETM:
		send_DATA_on_bus(request->addr,request->src_mid);
		state = MESI_CACHE_I;
		//Sim->bus->shared_line = false;
		break;
    case DATA:
    	//We have the modified data. should not see it.    	
		request->print_msg (my_table->moduleID, "ERROR");
		fatal_error("Should only have one outstanding request per processor!");
    	break;    	
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

