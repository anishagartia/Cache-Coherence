#ifndef ENUMS
#define ENUMS

typedef enum {
    CACHE_DATA = 1,
    CACHE_TAG,
} cache_type_t;

typedef enum {
    MSI = 1,
    MESI,
    MOSI,
} coherence_policy_t;

typedef enum {
    CACHE_PRO = 0,
    MI_PRO,
    MSI_PRO,
    MESI_PRO,
    MOESI_PRO,
    MOSI_PRO,
    MOESIF_PRO,
    NULL_PRO,
    MEM_PRO
} protocol_t;

typedef enum {
    TIER0 = 0,
    TIER1,
    TIER2,
    TIERMAX
} tier_t;

typedef enum {
    HASH_NODE_STRIDE,
    HASH_MC,
    HASH_DSDP_STYLE,
    HASH_INVALID
} hash_t;

typedef enum {
    QSET_FOLLOWER,
    QSET_1LVL,
    QSET_2LVL
} qset_t;


typedef enum {
	RP_LRU = 1
} replacement_policy_t;

typedef enum {
	CACHE_SHARED = 1,
	CACHE_PRIVATE
} cache_policy_t;

/** If node_t changes all config files need to be fixed.  */
typedef enum {
    NODE_VOID = 0,
    NODE_CORE,
    NODE_DIR,
    NODE_MC
} node_t;

typedef enum {
    L2SHARED = 1,
    L2PRIVATE
} cache_mode_t;

typedef enum {
    DIR_1L = 1,
    DIR_2L_FULL_REP,
    DIR_2L_SELECT_REP,
} dir_mode_t;

typedef enum {
	OUTPUT_FMT_COUT = 0,
	OUTPUT_FMT_CERR,
	OUTPUT_FMT_CSV,
	OUTPUT_FMT_NONE
} sim_output_mode_t;

typedef enum {
    MESH = 1,
    EXPRESS_MESH,
    TORUS
} network_topology_t;

typedef enum {
    INVALID_PRED = 0,
    THRESHOLD_PRED
} predictor_t;

typedef enum {
    INVALID_SCOPE = 0,
    NODE_SCOPE,
    NHOOD_SCOPE,
    GLOBAL_SCOPE
} predictor_scope_t;

typedef enum {
    INVALID_MAP = 0,
    ROUND_ROBIN_MAP,
    SEQUENTIAL_MAP
} thread_map_t;

#endif
