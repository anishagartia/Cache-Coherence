#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../sim/module.h"
#include "../sim/mreq.h"

class Hash_table;
class Sharers;

/** This is the base class for all Coherence Protocols
 * All of your protocols will inherit from this class
 */

class Protocol
{
public:

	/** This is a pointer to the cache the protocol belongs to */
    Hash_table *my_table;
    /** This is a pointer to the cache entry the protocol was called on */
    Hash_entry *my_entry;

    Protocol (Hash_table *my_table, Hash_entry *my_entry);
    virtual ~Protocol();

    /** This virtual function must be implemented by all children
     * This function handles requests that come from the processor
     */
    virtual void process_cache_request (Mreq *request) =0;
    /** This virtual function must be implemented by all children
	 * This function handles requests that come from the bus
	 */
    virtual void process_snoop_request (Mreq *request) =0;
    /** This virtual function must be implemented by all children
	 * This function dumps the coherence state (Useful for debugging)
	 */
    virtual void dump (void) =0;  

    /** These helper functions are provided to you to make it easier to
     * interface with the processor and bus.
     * If you need to add more messages you should add more sender functions
     * We suggest that you use these functions as a guideline if you wish to add
     * more sender functions.
     */
    void send_GETM(paddr_t addr);
    void send_GETS(paddr_t addr);
    void send_DATA_on_bus(paddr_t addr, ModuleID dest);
    void send_DATA_to_proc(paddr_t addr);
    /** These helper functions are for setting and getting the bus' shared line */
    void set_shared_line();
    bool get_shared_line();
};

#endif /* PROTOCOL_H_ */
