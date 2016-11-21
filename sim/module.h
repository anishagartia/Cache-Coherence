#ifndef MODULE_H_
#define MODULE_H_

#include <assert.h>
#include <iostream>
#include <map>

#include "settings.h"
#include "types.h"



using namespace std;

#define IO_PORT std::multimap<timestamp_t, Mreq *>

/** Order here is important!!!  Determines the links between tiers in the protocol.
 *  See Sim::get_directory_module_index() to see why. */
typedef enum {
    NI_M = 0,
    PR_M,
    L1_M,
    L2_M,
    L3_M,
    MC_M,
    INVALID_M
} module_t;

class ModuleID {
public:
    int nodeID;
    module_t module_index;

    bool operator== (const ModuleID &mid);
    bool operator!= (const ModuleID &mid);

    Module* get_module();
};

class Bus;

class Module {
public:
    char *name;
    ModuleID moduleID;

	Module (ModuleID moduleID, const char *name);
	virtual ~Module();

 	Mreq *read_input_port (void);
    bool write_output_port (Mreq *mreq);

    virtual void tick (void) =0;
    virtual void tock (void) =0;
};

void print_id (const char *str, ModuleID mid);

#endif // MODULE_H_
