#ifndef MEM_MAIN_H_
#define MEM_MAIN_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "module.h"
#include "mreq.h"
#include "settings.h"

using namespace std;

class Memory_controller : public Module
{
public:
	Memory_controller(ModuleID moduleID, int hit_time);
	~Memory_controller();

    int hit_time;

    bool request_in_progress;
    timestamp_t data_time;
    paddr_t data_addr;
    ModuleID data_target;

	void tick();
	void tock();
};

#endif /* MEM_MAIN_H_ */
