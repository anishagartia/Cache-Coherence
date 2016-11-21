#ifndef _MOESIF_CACHE_H
#define _MOESIF_CACHE_H

#include "../sim/types.h"
#include "../sim/enums.h"
#include "../sim/module.h"
#include "../sim/mreq.h"
#include "protocol.h"

/** Cache states.  */
typedef enum {
    MOESIF_CACHE_I = 1,
    MOESIF_CACHE_S,
    MOESIF_CACHE_E,
    MOESIF_CACHE_O,
    MOESIF_CACHE_M,
    MOESIF_CACHE_F,
	MOESIF_CACHE_IS,
	MOESIF_CACHE_IM,
	MOESIF_CACHE_SM,
	MOESIF_CACHE_OM,
	MOESIF_CACHE_FM,
} MOESIF_cache_state_t;

class MOESIF_protocol : public Protocol {
public:
    MOESIF_protocol (Hash_table *my_table, Hash_entry *my_entry);
    ~MOESIF_protocol ();

    MOESIF_cache_state_t state;
    
    void process_cache_request (Mreq *request);
    void process_snoop_request (Mreq *request);
    void dump (void);

    inline void do_cache_F (Mreq *request);
    inline void do_cache_I (Mreq *request);
    inline void do_cache_S (Mreq *request);
    inline void do_cache_E (Mreq *request);
    inline void do_cache_O (Mreq *request);
    inline void do_cache_M (Mreq *request);
	inline void do_cache_IS (Mreq *request);
	inline void do_cache_IM (Mreq *request);
	inline void do_cache_SM (Mreq *request);
	inline void do_cache_OM (Mreq *request);
	inline void do_cache_FM (Mreq *request);

    inline void do_snoop_F (Mreq *request);
    inline void do_snoop_I (Mreq *request);
    inline void do_snoop_S (Mreq *request);
    inline void do_snoop_E (Mreq *request);
    inline void do_snoop_O (Mreq *request);
    inline void do_snoop_M (Mreq *request);
	inline void do_snoop_IS (Mreq *request);
	inline void do_snoop_IM (Mreq *request);
	inline void do_snoop_SM (Mreq *request);
	inline void do_snoop_OM (Mreq *request);
	inline void do_snoop_FM (Mreq *request);
};

#endif // _MOESIF_CACHE_H
