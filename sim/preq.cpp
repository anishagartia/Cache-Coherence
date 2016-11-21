#include <assert.h>
#include <stdio.h>

#include "mreq.h"
#include "preq.h"
#include "sim.h"

extern Simulator Sim;

using namespace std;

/***************
 * Constructors.
 ***************/
Preq::Preq ()
{
    this->mid = (ModuleID) {(int) -1, (module_t)-1};
    this->pc = 0x0;
    this->addr = 0x0;
    this->msg = MREQ_MESSAGE_NUM;
    this->req_time = Global_Clock;
    this->resolve_time = 0;
    this->resolved = false;
    this->req_count = 0;
    this->history.clear();
    this->request_latencies[NI_M] = 0;
    this->request_latencies[PR_M] = 0;
    this->request_latencies[L1_M] = 0;
    this->request_latencies[L2_M] = 0;
    this->request_latencies[L3_M] = 0;
    this->request_latencies[MC_M] = 0;

}

Preq::Preq (ModuleID id, Processor_context *context, paddr_t pc, paddr_t addr, message_t msg)
{
    this->mid = id;
    this->context = context;
    this->pc = pc;
    this->addr = addr;
    this->msg = msg;
    this->req_time = Global_Clock;
    this->resolve_time = 0;
    this->resolved = false;
    this->req_count = 1;
    this->history.clear();
    this->request_latencies[NI_M] = 0;
    this->request_latencies[PR_M] = 0;
    this->request_latencies[L1_M] = 0;
    this->request_latencies[L2_M] = 0;
    this->request_latencies[L3_M] = 0;
    this->request_latencies[MC_M] = 0;
}

Preq::~Preq ()
{
    LIST <Preq_entry>::iterator it;

    for(it = history.begin(); it != history.end(); it++)
    {
        delete (*it).request;
        if ((*it).msg)
            free ((*it).msg);
    }

    history.clear();
}

void Preq::add_to_history (Mreq* request, const char *msg)
{
    Preq_entry entry;

    entry.time = Global_Clock;
    entry.request = request;
    entry.msg = (msg != NULL) ? strdup (msg) : NULL;

    history.push_back (entry);
}

void Preq::dump ()
{
    fprintf (stderr, "%s at 0x%x\tTimestamp %d\tRequest Count %d \t%d Resolve Time %d\n",
             (msg == C_GETIS) ? "C_GETIS" : "C_GETIM",
             (unsigned int) addr, (int) req_time, req_count, (int)resolved, (int) resolve_time );
    print_history ();
}

void Preq::print_history ()
{
    LIST <Preq_entry>::iterator it;

    for (it = history.begin(); it != history.end(); it++)
    {
        assert((*it).request != NULL);
        if ((*it).msg == NULL)
            ((*it).request)->preq_print_mreq ((*it).time, "Preq History");
        else
            ((*it).request)->preq_print_mreq ((*it).time, (*it).msg);
    }
}

/** Calculates latencies for each level of the memory hierarchy.
    TODO: Currently assumes only L1, L2, Mem.  */
void Preq::calculate_latencies (timestamp_t complete)
{
    Mreq *request;
    timestamp_t pr_l1, l1_l2, l2_mem, mem_l2, l2_l1, l1_pr;

    pr_l1 = l1_l2 = l2_mem = mem_l2 = l2_l1 = l1_pr = 0;
    for (LIST<Preq_entry>::iterator it = history.begin(); it != history.end(); it++)
    {
        assert((*it).request != NULL);
        request = (*it).request;

        /** PR -> L1  */
        if (request->src_mid.module_index == PR_M && request->dest_mid.module_index == L1_M  &&
            (request->msg == C_GETIS || request->msg == C_GETIM || request->msg == C_GETSM))
        {
            pr_l1 = request->req_time;
           // fprintf (stderr, "pr_l1 - %lu\n", request->req_time);
            continue;
        }

        /** L1 -> L2  */
        if (request->src_mid.module_index == L1_M && request->dest_mid.module_index == L2_M &&
            (request->msg == C_GETIS || request->msg == C_GETIM || request->msg == C_GETSM))
        {
            l1_l2 = request->req_time;
           // fprintf (stderr, "l1_l2 - %lu\n", request->req_time);
            continue;
        }

        /** L2 -> Mem  */
        if (request->src_mid.module_index == L2_M && request->dest_mid.module_index == MC_M &&
            (request->msg == C_GETIS || request->msg == C_GETIM || request->msg == C_GETSM))
        {
            l2_mem = request->req_time;
           // fprintf (stderr, "l2_mem - %lu\n", request->req_time);
            continue;
        }

        /** Mem -> L2  */
        if (request->src_mid.module_index == MC_M && request->dest_mid.module_index == L2_M &&
            (request->msg == C_DATA_S || request->msg == C_DATA_E || request->msg == C_DATA_M))
        {
            mem_l2 = request->req_time;
           // fprintf (stderr, "mem_l2 - %lu\n", request->req_time);
            continue;
        }

        /** (L1,L2) -> L1  */
        if ((request->src_mid.module_index == L1_M || request->src_mid.module_index == L2_M) &&
            request->dest_mid.module_index == L1_M &&
            (request->msg == C_DATA_S || request->msg == C_DATA_E || request->msg == C_DATA_M))
        {
            l2_l1 = request->req_time;
           // fprintf (stderr, "l2_l1 - %lu\n", request->req_time);
            continue;
        }

        /** L1 -> Pr  */
        if (request->src_mid.module_index == L1_M && request->dest_mid.module_index == PR_M &&
            (request->msg == C_DATA_S || request->msg == C_DATA_E || request->msg == C_DATA_M))
        {
            l1_pr = request->req_time;
           // fprintf (stderr, "l1_pr - %lu\n", request->req_time);
            continue;
        }
    }

    if (l1_l2 == 0)
    {
        request_latencies[L1_M] = (complete - pr_l1);
    }
    else if (l2_mem == 0)
    {
        request_latencies[L1_M] = (l1_l2 - pr_l1) + (complete - l1_pr);
        request_latencies[L2_M] = (l2_l1 - l1_l2);
    }
    else
    {
        request_latencies[L1_M] = (l1_l2 - pr_l1) + (complete - l1_pr);
        request_latencies[L2_M] = (l2_mem - l1_l2) + (l1_pr - l2_l1);
        request_latencies[MC_M] = l2_l1 - l2_mem;
    }

    //fprintf (stderr, "L1 - %lu\n", request_latencies[L1_M]);
    //fprintf (stderr, "L2 - %lu\n", request_latencies[L2_M]);
    //fprintf (stderr, "MC - %lu\n", request_latencies[MC_M]);
}
