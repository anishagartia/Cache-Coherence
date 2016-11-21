# Cache Coherence

Cache Coherence is the property where all caches simply must see all operations on a piece of data in the same order. In this project, we create a simulator that maintains coherent caches for a 4,8, and 16 core CMP. We have implemented the MESI, MOSI and MOESIF protocols for a bus-based broadcast system.

We implement the following Cache Coherence Protocols:

1. MI
2. MSI
3. MOSI
4. MESI
5. MOESI
6. MOESIF

## Specification of the Simulator:

The simulator is capable of simulating a 4, 8, and 16 core CMP system. Each core will consist of a memory trace reader. This trace reader will reads in trace files. Each core in the CMP has one level of cache. The cache is fully associative, has infinite size, and has a single cycle lookup time. The CMP has a single memory controller which can access the off chip memory. This memory controller responds to any query (GETS or GETM) placed on the bus with data after a 100 cycle delay. 

The bus modeled is an atomic bus. This means that once a query is placed on the bus, the bus will not allow any other requests onto the bus until a DATA response is seen. Caches request the bus using the bus_request() function. If the bus is not available, it will place the request on an arbitration queue to be scheduled in the future. This is done on a first come first served basis, with node 0 having the highest priority and node N having the lowest priority.

Each processor (trace reader) will have up to one outstanding memory request at a time. The processor will send a request to the cache and will wait until the cache responds with a DATA msg. (This will be done using the send_DATA_to_proc() command)
The simulator framework is in C++. It creates the simulator, reads in the traces, creates a basic cache structure, and creates the memory controller. All of this code is in the sim/ directory. 

The protocols/ directory contains files that implement the various cache coherence protocol. When a request comes from a processor to the cache, the cache finds the entry and then calls process_cache_request() in the protocol. It is in this function that we at the cache entry’s state and decide what messages (if any) should be sent, and what state the cache should transition to. When a request is snooped on the bus, the cache finds the entry and then calls process_snoop_request() in the protocol. It is in this function that you should look at the cache entry’s state and decided what messages (if any) should be sent, and what state the cache should transition to.

All requests that are not DATA (GETS and GETM) always expect to have someone reply with DATA. To ensure this, the memory will always respond 100 cycles after the request with DATA unless another cache places DATA on the bus first.

There are cases in the traditional protocol where there were certain messages that did not expect replies (e.g. Bus_Upgrade). These types of messages are not supported by the bus and memory, so we cannot use them. Instead we always send a query that expects a data response (e.g. GETS and GETM). This creates situations where the cache sending the GETS or GETM may be the one that should supply the data. In these cases, the cache simply sends DATA to itself on the bus.

## Statistics (Output)

1. Final cache coherence state (Already output by framework)
2. Number of cycles to complete execution (Already output by framework)
3. Number of cache misses (This can be due to a cold miss or coherence)
4. Number of cache accesses (Already output by framework)
5. Number of “silent upgrades” for the MESI (and extensions) protocol
6. Number of Cache-to-cache transfers (This refers to the number of times data is not supplied by Memory)
