#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <fstream>
#include <iostream>

#include "module.h"
#include "mreq.h"
#include "settings.h"
#include "types.h"

using namespace std;

class Hash_table;

class Processor : public Module {
public:
	Processor(ModuleID moduleID, Hash_table *cache, char *trace_file);
	~Processor();

    FILE *infile;
    Hash_table *my_cache;

    bool end_of_trace;
    bool outstanding_request;

    Mreq * inbound_request;
    Mreq * inbound_request_buf;

    bool done ();

	void tick ();
	void tock ();
};

#endif // PROCESSOR_H
