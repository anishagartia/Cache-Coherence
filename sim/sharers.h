#ifndef SHARERS_H
#define SHARERS_H

#include "settings.h"

#include <bitset>
using namespace std;

int abs_distance (int id1, int id2) ;

class Sharers {
public:
    Sharers ();
    virtual ~Sharers ();

    int owner;
    /** Warning: This blows...  */
    bitset<512> sharers;

    Sharers& operator= (Sharers sharers);

    int get_owner ();
    void set_owner (int nodeID);
    void clear_owner ();

    bool is_sharer (int nodeID);    
    int num_sharers (void);
    void add_sharer (int nodeID);
    void remove_sharer (int nodeID);
    void clear_sharers ();
    int nearest_sharer (int nodeID, bool owner_is_local_p);
    void dump_sharers (void);
};

#endif // SHARERS_H
