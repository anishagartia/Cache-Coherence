#include "node.h"
#include "processor.h"
#include "hash_table.h"
#include "memory.h"
#include "sim.h"

extern Sim_settings settings;
extern Simulator *Sim;

Node::Node (int nodeID)
{
    this->nodeID = nodeID;
    mod[L1_M] = NULL;
    mod[PR_M] = NULL;
    mod[MC_M] = NULL;
}

Node::~Node ()
{
    map<module_t, Module*>::iterator it;
    for (it = mod.begin (); it != mod.end (); it++)
        delete (it->second);
    mod.clear ();
}

void Node::build_processor (char *trace_file)
{
    Hash_table *cache;

    mod[L1_M] = cache = new Hash_table ((ModuleID){nodeID, L1_M}, "L1", 
                                        settings.l1_cache_size,
                                        settings.l1_cache_assoc,
                                        settings.cache_line_size,
                                        settings.l1_mshrs,
                                        settings.l1_hit_time,
                                        settings.protocol);

    mod[PR_M] = new Processor ((ModuleID){nodeID, PR_M}, cache, trace_file);
}

void Node::build_memory_controller (void)
{
	mod[MC_M] = new Memory_controller ((ModuleID){nodeID, MC_M}, 100);
}

void Node::tick_cache (void)
{
	if (mod[L1_M])
		mod[L1_M]->tick ();
}

void Node::tick_pr (void)
{
	if (mod[PR_M])
		mod[PR_M]->tick ();
}

void Node::tick_mc (void)
{
	if (mod[MC_M])
		mod[MC_M]->tick ();
}

void Node::tock_pr (void)
{
	if (mod[PR_M])
		mod[PR_M]->tock ();
}
