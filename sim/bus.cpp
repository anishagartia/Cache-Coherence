#include "bus.h"
#include "mreq.h"

Bus::Bus()
{
    current_request = NULL;
    data_reply = NULL;
    request_in_progress = false;
    shared_line = false;
}

Bus::~Bus()
{
}

void Bus::tick()
{
	if (current_request)
		delete current_request;

	if (request_in_progress)
	{
		if (data_reply)
		{
			current_request = data_reply;
			data_reply = NULL;
			request_in_progress=false;
		}
		else
		{
			current_request = NULL;
		}
	}
	else if (!pending_requests.empty())
	{
		shared_line = false;
	    current_request = pending_requests.front();
	    pending_requests.pop_front();
	    request_in_progress = true;
	}
	else
	{
		current_request = NULL;
	}
}

bool Bus::bus_request(Mreq *request)
{
	if (request->msg == DATA)
	{
		assert (data_reply == NULL);
		data_reply = request;
	}
	else
    {
        pending_requests.push_back(request);
    }

	return true;
}

Mreq* Bus::bus_snoop()
{
    Mreq *request;

    if (current_request)
    {
        request = new Mreq ();
        *request = *current_request;
        return request;
    }
    else
    {
        return NULL;
    }
}
