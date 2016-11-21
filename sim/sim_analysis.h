#ifndef SIM_ANALYSIS_H
#define SIM_ANALYSIS_H

#include "module.h"
#include "types.h"

using namespace std;

class Mreq;
class ModuleID;

/** 
 * Reference stream tracker.
 */
class Ref_stream_entry {
public:
    Ref_stream_entry (message_t msg, paddr_t addr, ModuleID mid);
    ~Ref_stream_entry ();

    paddr_t addr;
    ModuleID mid;
    message_t msg;   
};

class Reference_stream_tracker {
public:
    Reference_stream_tracker (paddr_t ref_stream_addr, int ref_stream_max);
    ~Reference_stream_tracker ();

    unsigned int ref_stream_max;
    paddr_t ref_stream_addr;
    LIST<Ref_stream_entry*> ref_stream;

    void update (Mreq *request);
    void dump ();
};

/** 
 * Read-only data tracker.
 */
class Read_only_tracker {
public:
    Read_only_tracker (int granularity, int max_entries);
    ~Read_only_tracker ();

    paddr_t addr_mask;
    unsigned int max_entries;
    MAP<paddr_t, bool> read_only_map;

    void update (Mreq *request);
    int get_nentries (void);
    int get_ro_cnt (void);
    int get_write_cnt (void);
    void dump ();
};

#endif // SIM_ANALYSIS_H
