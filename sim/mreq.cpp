#include <assert.h>
#include <stdio.h>

#include "mreq.h"
#include "settings.h"
#include "sim.h"

extern Sim_settings settings;
extern Simulator *Sim;

using namespace std;

/***************
 * Constructor.
 ***************/
Mreq::Mreq (message_t msg, paddr_t addr, ModuleID src_mid, ModuleID dest_mid)
{
    this->msg = msg;
    this->addr = addr & ((~0x0) << settings.cache_line_size_log2);
    this->src_mid = src_mid;
    this->dest_mid = dest_mid;
    this->fwd_mid = (ModuleID){-1,INVALID_M};
    this->INV_ACK_count = 0;
    this->req_time = Global_Clock;
    this->stalled = false;
    this->preq =NULL;
}

Mreq::~Mreq(void)
{
}

void Mreq::print_msg (ModuleID mid, const char *add_msg)
{
    //TODO: convert fprintfs to c++-ishy output
    print_id ("node", mid);
    print_id ("src", src_mid);
    print_id ("dest", dest_mid);
    fprintf (stderr, "tag: 0x%8llx clock: %8lld ", (long long int)addr>>settings.cache_line_size_log2, Global_Clock);
    fprintf (stderr, " %8s\n", Mreq::message_t_str[msg]);
}

void Mreq::dump ()
{
    //TODO: convert fprintfs to c++-ishy output
    fprintf (stderr, "Request Dump ");
    print_id ("src", src_mid);
    print_id ("dest", dest_mid);
    fprintf (stderr, "0x%8llx Clock: %8lld %20s\n",
             (long long int)addr, Global_Clock, Mreq::message_t_str[msg]);
}
