#ifndef SIM_H
#define SIM_H

#include "bus.h"
#include "enums.h"
#include "node.h"
#include "settings.h"
#include "types.h"

#define Global_Clock Sim->global_clock

class Node;
class Processor;
class Hash_table;
class L1_cache;
class Memory_controller;

void fatal_error (const char *fmt, ...) __attribute__ ((noreturn));

class Simulator {
public:
    Simulator ();
    ~Simulator ();

    timestamp_t global_clock;

    Node **Nd;
    Bus *bus;

    /** Run/Fini for simulator.  */
    void run (void);
    void dump_stats (void);

    /** Accessor functions */
    Processor *get_PR (int node);
    Hash_table *get_L1 (int node);
    Memory_controller *get_MC (int node);

    /** Debug.  */
    void dump_processors (void);
	void dump_outstanding_requests (int nodeID);
    void dump_cache_block (int nodeID, paddr_t addr);

    unsigned long int cache_misses;
    unsigned long int cache_accesses;
    unsigned long int silent_upgrades;
    unsigned long int cache_to_cache_transfers;
};

#endif

