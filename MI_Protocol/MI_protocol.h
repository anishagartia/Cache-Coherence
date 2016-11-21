#ifndef _MI_CACHE_H
#define _MI_CACHE_H

#include "../sim/types.h"
#include "../sim/enums.h"
#include "../sim/module.h"
#include "../sim/mreq.h"
#include "protocol.h"

/** Cache states.  */
typedef enum {
    MI_CACHE_I = 1,
    MI_CACHE_IM,
    MI_CACHE_M,
} MI_cache_state_t;

class MI_protocol : public Protocol {
public:
    MI_protocol (Hash_table *my_table, Hash_entry *my_entry);
    ~MI_protocol ();

    // Cache state for this line
    MI_cache_state_t state;
    
    void process_cache_request (Mreq *request);
    void process_snoop_request (Mreq *request);
    void dump (void);

    /* Functions that specify the actions to take on requests from the processor
     * when the cache is in various states
     */
    inline void do_cache_I (Mreq *request);
    inline void do_cache_IM (Mreq * request);
    inline void do_cache_M (Mreq *request);

    /* Functions that specify the actions to take on snooped requests
     * when the cache is in various states
     */
    inline void do_snoop_I (Mreq *request);
    inline void do_snoop_IM (Mreq * request);
    inline void do_snoop_M (Mreq *request);
};

#endif // _MI_CACHE_H
