#ifndef _MOSI_CACHE_H
#define _MOSI_CACHE_H

#include "../sim/types.h"
#include "../sim/enums.h"
#include "../sim/module.h"
#include "../sim/mreq.h"
#include "protocol.h"

/** Cache states.  */
typedef enum {
    MOSI_CACHE_I = 1,
    MOSI_CACHE_S,
    MOSI_CACHE_O,
    MOSI_CACHE_M,
	MOSI_CACHE_IM,
	MOSI_CACHE_IS,
	MOSI_CACHE_SM,
	MOSI_CACHE_OStr	
} MOSI_cache_state_t;

class MOSI_protocol : public Protocol {
public:
    MOSI_protocol (Hash_table *my_table, Hash_entry *my_entry);
    ~MOSI_protocol ();

    MOSI_cache_state_t state;
    
    void process_cache_request (Mreq *request);
    void process_snoop_request (Mreq *request);
    void dump (void);

    inline void do_cache_I (Mreq *request);
    inline void do_cache_S (Mreq * request);
    inline void do_cache_O (Mreq * request);
    inline void do_cache_M (Mreq *request);
	inline void do_cache_IS (Mreq *request);
	inline void do_cache_IM (Mreq *request);
	inline void do_cache_SM (Mreq *request);
	inline void do_cache_OStr (Mreq *request);

    inline void do_snoop_I (Mreq *request);
    inline void do_snoop_S (Mreq *request);
    inline void do_snoop_O (Mreq *request);
    inline void do_snoop_M (Mreq *request);
	inline void do_snoop_IS (Mreq *request);
	inline void do_snoop_IM (Mreq *request);
	inline void do_snoop_SM (Mreq *request);
	inline void do_snoop_OStr (Mreq *request);
};

#endif // _MOSI_CACHE_H
