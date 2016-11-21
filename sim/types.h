#ifndef TYPES_H_
#define TYPES_H_

#include <algorithm>
#include <deque>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <stdint.h>
#include <cstdio>

using namespace std;

#define DEQUE deque
#define LIST list
#define SET set
#define MAP map
#define QUEUE queue
#define STACK stack
#define VECTOR vector

typedef uint64_t paddr_t;
typedef uint64_t timestamp_t;
typedef uint64_t counter_t;

class Hash_table;
class Hash_set;
class Hash_entry;
class L1_cache;
class L2_cache;
class Directory;
class Directory_entry;
class Memory_controller;
class Module;
class ModuleID;
class Mreq;
class Node;
class Preq;
class processor;
class Processor_context;
class Sharers;
class Simulator;
class Stat_engine;
class Hash_table_stat_engine;
class Directory_stat_engine;
class Memory_controller_stat_engine;
class Network_interface_stat_engine;
class Processor_stat_engine;
class Simulator_stat_engine;
class Router_stat_engine;
class Sim_stat_manager;

#endif
