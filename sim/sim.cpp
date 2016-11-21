#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>

#include "hash_table.h"
#include "processor.h"
#include "memory.h"
#include "module.h"
#include "mreq.h"
#include "settings.h"
#include "sim.h"
#include "types.h"

extern Sim_settings settings;

/** Fatal Error.  */
void fatal_error (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end (ap);
    
    /** Enable debugging by asserting zero.  */
    assert (0 && "Fatal Error");
    exit (-1);
}

Simulator::Simulator ()
{
    /** Seed random number generator.  */
    srandom (1023);
    
    /** Set global_clock to cycle zero.  */
    global_clock = 0;

    /** Allocate bus.  */
    bus = new Bus ();
    assert (bus && "Sim error: Unable to alloc bus.");

    Nd = new Node*[settings.num_nodes+1];

    /** Allocate processors.  */
    for (int node = 0; node < settings.num_nodes; node++)
    {
        char trace_file[100];
        sprintf (trace_file, "%s/p%d.trace", settings.trace_dir, node);

        Nd[node] = new Node (node);
        Nd[node]->build_processor (trace_file);
    }

    /** Allocate memory controllers.  */
    Nd[settings.num_nodes] = new Node (settings.num_nodes);
    Nd[settings.num_nodes]->build_memory_controller ();

    cache_misses = 0;
    silent_upgrades = 0;
    cache_to_cache_transfers = 0;
    cache_accesses = 0;
}

Simulator::~Simulator ()
{
    for (int i = 0; i < settings.num_nodes; i++)
        delete Nd[i];

    delete [] Nd;    
}

void Simulator::dump_stats ()
{
    for (int i=0; i < settings.num_nodes; i++)
    {
    	get_L1(i)->dump_hash_table();
    }
    fprintf(stderr,"\nRun Time:         %8lld cycles\n",global_clock);
    fprintf(stderr,"Cache Misses:     %8ld misses\n",cache_misses);
    fprintf(stderr,"Cache Accesses:   %8ld accesses\n",cache_accesses);
    fprintf(stderr,"Silent Upgrades:  %8ld upgrades\n",silent_upgrades);
    fprintf(stderr,"$-to-$ Transfers: %8ld transfers\n",cache_to_cache_transfers);
}

void Simulator::run ()
{
    int sched;
    bool done;

    /** This must match what's in enums.h.  */
    const char *cp_str[9] = {"CACHE_PRO","MI_PRO","MSI_PRO","MESI_PRO",
							 "MOESI_PRO","MOSI_PRO","MOESIF_PRO","NULL_PRO","MEM_PRO"};

    fprintf (stderr, "CSX290 Sim - Begins  ");
    fprintf (stderr, " Cores: %d", settings.num_nodes);
    fprintf (stderr, " Protocol: %s\n", cp_str[settings.protocol]);

    /** Main run loop.  */
    sched = 0;
    done = false;
    while (!done)
    {
        bus->tick ();

        for (int i = 0; i <= settings.num_nodes; i++)
            Nd[i]->tick_cache ();

        for (int i = 0; i <= settings.num_nodes; i++)
            Nd[i]->tick_pr ();

        for (int i = 0; i <= settings.num_nodes; i++)
            Nd[i]->tick_mc ();
        
        for (int i = 0; i <= settings.num_nodes; i++)
			Nd[i]->tock_pr ();

        global_clock++;

        done = true;
        for (int i = 0; i < settings.num_nodes; i++)
            if (!get_PR(i)->done ())
            {
                done = false;
                break;        
            }
    }

    fprintf(stderr,"\n\nSimulation Finished\n");
    dump_stats();
}

Processor* Simulator::get_PR (int node)
{
    return (Processor *)(Nd[node]->mod[PR_M]);
}
Hash_table* Simulator::get_L1 (int node)
{
    return (Hash_table *)(Nd[node]->mod[L1_M]);
}
Memory_controller* Simulator::get_MC (int node)
{
    return (Memory_controller *)(Nd[node]->mod[MC_M]);
}

/** Debug.  */
void Simulator::dump_processors (void)
{
    Processor *pr;
    for (int i = 0; i < settings.num_nodes; i++)
    {
        pr = get_PR (i);
    }
}

void Simulator::dump_outstanding_requests (int nodeID)
{
    assert (nodeID < settings.num_nodes);   
}

void Simulator::dump_cache_block (int nodeID, paddr_t addr)
{
    assert (get_L1(nodeID));
    get_L1 (nodeID)->dump_hash_entry (addr);
}
