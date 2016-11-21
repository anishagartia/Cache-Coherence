#include "memory.h"
#include "sim.h"

extern Simulator * Sim;

Memory_controller::Memory_controller(ModuleID moduleID, int hit_time)
	: Module (moduleID, "MC_")
{
	this->hit_time = hit_time;
	request_in_progress = false;
	data_time = 0;
	data_target = (ModuleID){-1,INVALID_M};
}

Memory_controller::~Memory_controller()
{
}

void Memory_controller::tick()
{
    Mreq *request;

    if ((request = read_input_port ()) != NULL)
    {
		if (request->msg != DATA)
		{
			assert (!request_in_progress);
			request_in_progress = true;
			data_addr = request->addr;
			data_target = request->src_mid;
			data_time = Global_Clock + hit_time;
		}
		else
		{
			request_in_progress = false;
		}
    }

    if (request_in_progress && Global_Clock >= data_time)
    {
    	Mreq * new_request;
    	new_request = new Mreq(DATA,data_addr,moduleID,data_target);
    	request_in_progress = false;
    	fprintf(stderr,"**** DATA SEND MC -- Clock: %lld\n",Global_Clock);
    	this->write_output_port(new_request);
    }
}

void Memory_controller::tock()
{
    fatal_error ("Memory controller tock should never be called!\n");
}

