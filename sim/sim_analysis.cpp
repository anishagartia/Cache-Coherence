#include "mreq.h"
#include "sim.h"
#include "sim_analysis.h"

extern Simulator Sim;

/********************************************************************************
 * Reference stream tracker.  
 ********************************************************************************/
Ref_stream_entry::Ref_stream_entry (message_t msg, paddr_t addr, ModuleID mid)
{
    this->msg = msg;
    this->addr = addr;
    this->mid = mid;
}

Ref_stream_entry::~Ref_stream_entry ()
{
}

Reference_stream_tracker::Reference_stream_tracker (paddr_t ref_stream_addr, int ref_stream_max)
{
    this->ref_stream_addr = ref_stream_addr;
    this->ref_stream_max = ref_stream_max;
    ref_stream.clear ();
}

Reference_stream_tracker::~Reference_stream_tracker ()
{
}

void Reference_stream_tracker::update (Mreq *request)
{
    if (ref_stream.size () >= ref_stream_max)
        return;

    ref_stream.push_back (new Ref_stream_entry (request->msg, request->addr, request->src_mid));
}

void Reference_stream_tracker::dump ()
{
    LIST<Ref_stream_entry*>::iterator it;

    fprintf (stderr, "Reference Stream Tracker:\n");
    fprintf (stderr, "Addr: 0x%llx\n", (unsigned long long)ref_stream_addr);

    for (it = ref_stream.begin (); it != ref_stream.end (); it++)
    {
        //fprintf (stderr, "Msg: %s Node: %d", Mreq::message_t_str[it->second->msg], it->second->mid.NodeID);
    }
}

/********************************************************************************
 * Read-only data tracker.  
 ********************************************************************************/
Read_only_tracker::Read_only_tracker (int granularity, int max_entries)
{
    assert ((unsigned int)granularity >= Sim.settings.cache_line_size);
    assert (max_entries > 0);

    /** Calculate granularity mask.  */
    this->addr_mask = ~0x0;
    for ( ;granularity > 1; granularity /= 2)
        this->addr_mask <<= 1;

    this->max_entries = max_entries;
    this->read_only_map.clear ();
}

Read_only_tracker::~Read_only_tracker ()
{
    this->read_only_map.clear ();
}

void Read_only_tracker::update (Mreq *request)
{
    paddr_t addr;
    MAP<paddr_t, bool>::iterator it;

    addr = request->addr & addr_mask;

    it = read_only_map.find (addr);

    /** Not found.  */
    if (it == read_only_map.end ())
    {
        if (read_only_map.size () < max_entries)
            read_only_map[addr] = (request->msg == C_GETIS) ? true : false;
    }
    else
    {
        if (request->msg == C_GETIM || request->msg == C_GETSM)
            it->second = false;
    }        
}

int Read_only_tracker::get_nentries (void)
{
    return read_only_map.size ();
}

int Read_only_tracker::get_ro_cnt (void)
{
    unsigned int ro_cnt;
    MAP<paddr_t, bool>::iterator it;

    ro_cnt = 0;
    for (it = read_only_map.begin (); it != read_only_map.end (); it++)
        if (it->second == true)
            ro_cnt++;
    
    return ro_cnt;
}

int Read_only_tracker::get_write_cnt (void)
{
    unsigned int write_cnt;
    MAP<paddr_t, bool>::iterator it;

    write_cnt = 0;
    for (it = read_only_map.begin (); it != read_only_map.end (); it++)
        if (it->second == false)
            write_cnt++;

    return write_cnt;
}

void Read_only_tracker::dump (void)
{
    unsigned int ro_cnt;
    unsigned int write_cnt;
    MAP<paddr_t, bool>::iterator it;

    ro_cnt = 0;
    write_cnt = 0;
    for (it = read_only_map.begin (); it != read_only_map.end (); it++)
        if (it->second)
            ro_cnt++;
        else
            write_cnt++;
}
