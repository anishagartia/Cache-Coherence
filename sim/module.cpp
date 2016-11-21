#include <assert.h>
#include <string.h>
#include "bus.h"
#include "module.h"
#include "mreq.h"
#include "sim.h"
#include "types.h"

extern Simulator *Sim;

bool ModuleID::operator== (const ModuleID &mid)
{
    return (this->nodeID == mid.nodeID &&
            this->module_index == mid.module_index);
}

bool ModuleID::operator!= (const ModuleID &mid)
{
    return !(this->nodeID == mid.nodeID &&
             this->module_index == mid.module_index);
}

Module *ModuleID::get_module ()
{
    Module *ret = Sim->Nd[nodeID]->mod[module_index];
    assert (ret != NULL);
    return ret;
}

/************************************************************
 * Module constructor, destructor, and associated functions.
 ************************************************************/
Module::Module (ModuleID moduleID, const char *name)
{
    this->moduleID = moduleID;
    this->name = strdup (name);
}

Module::~Module (void)
{
    if (name)
        free (name);
}

Mreq *Module::read_input_port (void)
{
    return Sim->bus->bus_snoop ();
}

bool Module::write_output_port (Mreq *mreq)
{
    return Sim->bus->bus_request (mreq);
}

void print_id (const char *str, ModuleID mid)
{
    switch (mid.module_index) {
    case NI_M: fprintf (stderr, "%4s:%3d/NI  ", str, mid.nodeID); break;
    case PR_M: fprintf (stderr, "%4s:%3d/PR  ", str, mid.nodeID); break;
    case L1_M: fprintf (stderr, "%4s:%3d/L1  ", str, mid.nodeID); break;
    case L2_M: fprintf (stderr, "%4s:%3d/L2  ", str, mid.nodeID); break;
    case L3_M: fprintf (stderr, "%4s:%3d/L3  ", str, mid.nodeID); break;
    case MC_M: fprintf (stderr, "%4s:%3d/MC  ", str, mid.nodeID); break;
    case INVALID_M:  fprintf (stderr, "%4s:  None ", str); break;
    }
}

