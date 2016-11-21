#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <iostream>

#include "module.h"
#include "mreq.h"
#include "settings.h"
#include "types.h"
#include "../protocols/protocol.h"

using namespace std;

/** Individual entry for a hardware hash-like structure. */
class Hash_entry {
public:
    Hash_entry (Hash_table *t, paddr_t tag);
    virtual ~Hash_entry (void);

    Hash_table *my_table;
    paddr_t tag;

    Protocol *protocol;

    void process_request_snoop (Mreq *request);
    void process_request_processor (Mreq *request);

    /** Debug.  */
    void dump ();
};

class Hash_table: public Module {
public:
    /** Parameters.  */
    int size;
    int assoc;
    int sets;
    int blocksize;
    int mshrs;
    int hit_time;
    protocol_t protocol;

    /** Masks for tag, index.  */
    int num_index_bits;
    int num_offset_bits;
    paddr_t tag_mask;
    paddr_t index_mask;

    Mreq *proc_request;

    /** Table divided into sets which house the individual entries, indexed with index bits.  */
    MAP<paddr_t, Hash_entry*> my_entries;
    Hash_entry* null_entry;

    /** Internal helper functions.  */
    Hash_entry* get_entry (paddr_t addr);

public:
    Hash_table (ModuleID moduleID, const char *name,
                int size, int assoc, int blocksize, int mshrs,
                int hit_time, protocol_t protocol);
                
    ~Hash_table (void);

    void processor_request (Mreq *request);

    bool write_to_proc (Mreq *mreq);
    bool write_to_bus (Mreq *mreq);

    void tick (void);
    void tock (void);

    /** Debug.  */
    void print_config (void);
    void dump_hash_entry (paddr_t addr);
    void dump_hash_table ();
};

#endif /** HASH_TABLE_H_*/
