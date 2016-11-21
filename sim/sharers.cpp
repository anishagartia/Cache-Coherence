#include <assert.h>

#include "sharers.h"
#include "sim.h"
#include "settings.h"

extern Sim_settings settings;
extern Simulator *Sim;

/********************************
 * Constructor/destructor.
 ********************************/
Sharers::Sharers (void)
{
    if (settings.num_nodes > 512) 
    {
        fatal_error ("Sharers hard coded to 512 nodes..  fix .h file!");
    }
    owner = -1;
	sharers.reset();
}

Sharers::~Sharers (void)
{
}

Sharers& Sharers::operator= (Sharers sharers)
{
    this->clear_sharers ();
    for (int i = 0, j = sharers.num_sharers (); j > 0; i++)
    {
        if (sharers.is_sharer (i))
        {
            this->add_sharer (i);
            j--;
        }
    }
    this->set_owner (sharers.get_owner ());
    return *this;
}

int Sharers::get_owner (void)
{
    return owner;
}

void Sharers::set_owner (int nodeID)
{
    owner = nodeID;
}

void Sharers::clear_owner (void)
{
    owner = -1;
}

bool Sharers::is_sharer (int nodeID)
{
    return sharers.test (nodeID);
}
    
int Sharers::num_sharers (void) 
{
    return sharers.count ();
} 

void Sharers::add_sharer (int nodeID) 
{
    sharers.set (nodeID);
}

void Sharers::remove_sharer (int nodeID)
{
    sharers.reset (nodeID);
}

void Sharers::clear_sharers (void)
{
    if (sharers.any ())
        sharers.reset ();
}

void Sharers::dump_sharers (void)
{
    fprintf (stderr, "Dump Sharers:\n");
    fprintf (stderr, "Owner: %d ", owner);
    fprintf (stderr, "Sharers:");
    for (int i = 0; i < 512; i++)
    {
        if (sharers.test (i))
        {
            fprintf (stderr, " %d", i);
        }
    }
    fprintf (stderr, "\n");
}

/** Input node id, return nearest sharer DS style.  */

int Sharers::nearest_sharer (int id, bool owner_is_local_p)
{ 
#if 0
    //TODO:  Isn't used anymore (in MOSI cache-to-cache causes livelock)!  Delete it outright?

    int i; 
    int nearest = -1;  
    int distance = 0, distance_new = 0;
    int owner_distance;
 
    assert (num_sharers () > 0);

    //fprintf (stderr, "find nearest for: %d\n", id);

    if (num_sharers () == 0 && owner_is_local_p)
        return owner;

    /** Get first sharer and set to nearest.  */ 
    for (i = 0; i < settings.num_nodes; i++)
    { 
        if (i == id || settings.node_type[i] != NODE_CORE)
            continue;

        if (is_sharer (i)) 
        { 
            //fprintf (stderr, "%d - nearest: %d\n", id, i);
            nearest = i; 
            distance = abs_distance (id, i);  
            break;
        }
    }
 
    /** Compare against all other sharers.  */
    for ( ; i < settings.num_nodes; i++)
    {
        if (i == id || settings.node_type[i] != NODE_CORE)
            continue;

        if (is_sharer (i))
        {
            distance_new = abs_distance (id, i);  

            if (distance_new < distance)
            {
                //fprintf (stderr, "%d - nearest: %d\n", id, i);
                nearest = i;
                distance = distance_new;
            }
        }
    }
 
    /** Owner and no sharers.  */
    if (owner_is_local_p && owner != -1 && nearest == -1)
    {
        return owner;
    }
 
    /** Owner ties with sharer, return owner.  */
    if (owner_is_local_p && owner != -1) {
        owner_distance = abs_distance (id, i);  

        if (owner_distance <= distance)
        {
            return owner;
        }
    }

    //assert (nearest != -1);
    return nearest;
#endif
    fatal_error("nearest sharer code removed");
    return -1;
}

int abs_distance (int id1, int id2) 
{
    int id1_x_coor, id1_y_coor, id2_x_coor, id2_y_coor;
    int distance;
 
    id1_x_coor = id1 % settings.network_y_dimension;
    id1_y_coor = id1 / settings.network_y_dimension;
    id2_x_coor = id2 % settings.network_y_dimension;
    id2_y_coor = id2 / settings.network_y_dimension;
 
    distance = (id1_x_coor > id2_x_coor) ? id1_x_coor - id2_x_coor : id2_x_coor - id1_x_coor;
    distance += (id1_y_coor > id2_y_coor) ? id1_y_coor - id2_y_coor : id2_y_coor - id1_y_coor;
 
    return distance;
}

