#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sim.h"
#include "settings.h"
#include "enums.h"

extern Sim_settings settings;
extern FILE * yyin;

//TODO: this needs to be called to reclaim space allocated by flex, but yylex_destroy() doesn't exist
// for older versions of flex.  Pasta clusters have flex v2.5.4, linux distros currently have v2.5.35,
// and our macs has v.2.5.35
//extern int yylex_destroy();

extern Simulator Sim;

// Possible identifiers for config file
setts identifiers [] = {
    /** NOC.  */
    {"network_x_dimension",     &(settings.network_x_dimension)   },
    {"network_y_dimension",     &(settings.network_y_dimension)   },

    /** Neighborhoods.  */
    {"num_nhoods",              &(settings.num_nhoods)            },
    {"nhood_x_blocking_factor", &(settings.nhood_x_blocking_factor)},
    {"nhood_y_blocking_factor", &(settings.nhood_y_blocking_factor)},

    /** Memory controller.  */
    {"num_mem_ctrls",           &(settings.num_mem_ctrls)         },
    {"mem_ctrl_array",          &(settings.mem_ctrl_array)        },

	{"heartrate",               &(settings.heartrate)             },
	{"net_infinite_bw",		   	&(settings.net_infinite_bw)       },
	{"sharer_forwarding",	   	&(settings.sharer_forwarding)     },
	{"wait_on_inv_acks",	   	&(settings.wait_on_inv_acks)      },
	{"livelock_check",		   	&(settings.livelock_check)        },
	{"processor_affinity",		&(settings.processor_affinity)    },
    {"mem_model_enabled",       &(settings.mem_model_enabled)     },

    /** Is this a regression run?  */
    {"regression_test",         &(settings.regression_test)       },

    /** SESC specific.  */
	{"sesc_rabbit",			   	&(settings.sesc_rabbit)           },
    {"sesc_nsim_per_core",      &(settings.sesc_nsim_per_core)    },
    {"sesc_disable_llsc",       &(settings.sesc_disable_llsc)     },
	{"warmup_time_per_core",	&(settings.warmup_time_per_core)  },

    /** General cache.  */
	{"cache_line_size_log2",   	&(settings.cache_line_size_log2)  },
	{"cache_line_size",		   	&(settings.cache_line_size)       },

	/** Processor.  */
    {"LSQ_dependence",          &(settings.LSQ_dependence)         },
    {"mshrs_per_processor",     &(settings.mshrs_per_processor)    },
    {"threads_per_processor",   &(settings.threads_per_processor)  },
    {"thread_map_policy",       &(settings.thread_map_policy)      },

    /** Simple processor.  */
    {"simple_issue_width",      &(settings.simple_issue_width)     },

    /** Inorder processor.  */
    {"inorder_fetch_width",     &(settings.inorder_fetch_width)    },
    {"inorder_issue_width",     &(settings.inorder_issue_width)    },
    {"inorder_commit_width",    &(settings.inorder_commit_width)   },

    /** L1 cache.  */
    {"l1_cache_type",           &(settings.l1_cache_type)         },
	{"l1_cache_size",		   	&(settings.l1_cache_size)         },
	{"l1_cache_assoc",		   	&(settings.l1_cache_assoc)        },
	{"l1_hit_time",			   	&(settings.l1_hit_time)           },
	{"l1_mshrs",			   	&(settings.l1_mshrs)              },
	{"l1_replacement_policy",  	&(settings.l1_replacement_policy) },
	{"l1_lookup_time",		   	&(settings.l1_lookup_time)        },
	{"l1_infinite",		   	    &(settings.l1_infinite)           },

    /** L2 cache.  */
    {"l2_cache_type",           &(settings.l2_cache_type)         },
	{"l2_cache_size",		   	&(settings.l2_cache_size)         },
	{"l2_cache_assoc",		   	&(settings.l2_cache_assoc)        },
	{"l2_hit_time",			   	&(settings.l2_hit_time)           },
	{"l2_mshrs",			   	&(settings.l2_mshrs)              },
	{"l2_replacement_policy",  	&(settings.l2_replacement_policy) },
	{"l2_lookup_time",		 	&(settings.l2_lookup_time)        },
	{"l2_infinite",		   	    &(settings.l2_infinite)           },

    /** L3 cache.  */
    {"l3_cache_type",           &(settings.l3_cache_type)         },
	{"l3_cache_size",		   	&(settings.l3_cache_size)         },
	{"l3_cache_assoc",		   	&(settings.l3_cache_assoc)        },
	{"l3_hit_time",			   	&(settings.l3_hit_time)           },
	{"l3_mshrs",			   	&(settings.l3_mshrs)              },
	{"l3_replacement_policy",  	&(settings.l3_replacement_policy) },
	{"l3_lookup_time",		 	&(settings.l3_lookup_time)        },
	{"l3_infinite",		   	    &(settings.l3_infinite)           },

    /** Directory.  */
	{"dir_tiers",			    &(settings.dir_tiers)             },
	{"dir_coherence_policy",	&(settings.dir_coherence_policy)  },
    {"dir_mode",                &(settings.dir_mode)              },
    
    /** Make sure home bits don't overlap with index bits.  */
    {"dir_addr_per_node_log2",  &(settings.dir_addr_per_node_log2)},

    /** Set index and directory home node swizzle.  */
	{"cache_index_swizzle",	    &(settings.cache_index_swizzle)   },
	{"dir_home_swizzle",	    &(settings.dir_home_swizzle)      },

    /** Dynamic home node remapping.  */
    {"qsets_enabled",           &(settings.qsets_enabled)         },
    {"qsets_interval",          &(settings.qsets_interval)        },
    {"remap_table_size",        &(settings.remap_table_size)      },

    /** Selective Replication predictor.  */
    {"sel_rep_pred",            &(settings.sel_rep_pred)          },
    {"sel_rep_pred_scope",      &(settings.sel_rep_pred_scope)    },
    {"train_on_loads",          &(settings.train_on_loads)        },
    {"train_on_stores",         &(settings.train_on_stores)       },
    {"sel_rep_pred_threshold",  &(settings.sel_rep_pred_threshold)},

    /** Sim Analysis flags.  */
    {"sim_analysis_enabled",    &(settings.sim_analysis_enabled)  },
    {"ro_tracker_gran",         &(settings.ro_tracker_gran)       },
    {"ro_tracker_entries",      &(settings.ro_tracker_entries)    },
	{"data_graph",				&(settings.data_graph)			  },


	/** Express Link and VC Stuff */
    {"network_topology",        &(settings.network_topology)      },
	{"express_link_len",		&(settings.express_link_len)	  },
	{"express_link_active",		&(settings.express_link_active)	  },

	/** DO NOT SET IN CONFIG FILE: These are set automagically by net_infinite_bw **/
	{"num_virtual_channels",	&(settings.num_virtual_channels)  },
	{"buffer_entries_per_vc",	&(settings.buffer_entries_per_vc) },
	{"debug_addr",	            &(settings.debug_addr)            },
    {"test_addr",               &(settings.test_addr)            },

	/** report generation, tell simulator to output to cerr, cout, or null for no output **/
	{"report_output",           &(settings.report_output)         },

	/** Sampling Rate for statistics that are collected in intervals (i.e. avg sharer stat **/
	{"sampling_interval",		&(settings.sampling_interval)	  },

    /** Invalid.  */
    {"end",						NULL                                  }
};

Sim_settings::Sim_settings (void)
{
    mem_ctrl_array = NULL;
}

Sim_settings::~Sim_settings (void)
{
    if (mem_ctrl_array)
        delete [] mem_ctrl_array;

    //TODO: reenable this after we get a newer version of flex on pasta?
    //free resources claimed by flex
    //yylex_destroy();
}

void Sim_settings::print_settings (void) 
{
    fprintf (stderr, "SIM Settings:\n");
	fprintf (stderr, " num_nodes:             %16d\n", num_nodes);
	fprintf (stderr, " network_x_dimension:   %16d\n", network_x_dimension);
	fprintf (stderr, " network_y_dimension:   %16d\n", network_y_dimension);

	fprintf (stderr, " num_nhoods:            %16d\n", num_nhoods);
    fprintf (stderr, "nhood_x_blocking_factor %16d\n", nhood_x_blocking_factor);
    fprintf (stderr, "nhood_y_blocking_factor %16d\n", nhood_y_blocking_factor);

	fprintf (stderr, " num_mem_ctrls:         %16d\n", num_mem_ctrls);


	fprintf (stderr, " net_infinite_bw:       %16s\n", net_infinite_bw == true ? "true" : "false");
	fprintf (stderr, " sharer_forwarding:     %16s\n", sharer_forwarding == true ? "true" : "false");
	fprintf (stderr, " wait_on_inv_acks:      %16s\n", wait_on_inv_acks == true ? "true" : "false");
	fprintf (stderr, " livelock_check:        %16s\n", livelock_check == true ? "true" : "false");
    fprintf (stderr, " heartrate              %16d\n", heartrate);
	fprintf (stderr, " processor_affinity:    %16s\n", processor_affinity == true ? "true" : "false");
    fprintf (stderr, " mem_model_enabled:     %16s\n", mem_model_enabled == true ? "true" : "false");
	fprintf (stderr, " regression_test:       %16s\n", regression_test == true ? "true" : "false");

	fprintf (stderr, " sesc_rabbit:           %16lld\n", sesc_rabbit);
	fprintf (stderr, " sesc_nsim:             %16lld\n", sesc_nsim);
	fprintf (stderr, " sesc_nsim_per_core:    %16lld\n", sesc_nsim_per_core);
    fprintf (stderr, " sesc_disable_llsc:     %16s\n", sesc_disable_llsc == true ? "true" : "false");

	fprintf (stderr, " warmup_time:			  %16lld\n", warmup_time);
	fprintf (stderr, " warmup_time_per_core:  %16lld\n", warmup_time_per_core);

	fprintf (stderr, " cache_line_size_log2:  %16d\n", cache_line_size_log2);
	fprintf (stderr, " cache_line_size:       %16d\n", cache_line_size);

	fprintf (stderr, " LSQ_dependence:        %16d\n", LSQ_dependence);
    fprintf (stderr, " mshrs_per_processor:   %16d\n", mshrs_per_processor);
    fprintf (stderr, " threads_per_processor: %16d\n", threads_per_processor);
    fprintf (stderr, " thread_map_policy:     %16d\n", thread_map_policy);

    /** Simple processor.  */
    fprintf (stderr, " simple issue width     %16d\n", simple_issue_width);

    /** Inorder processor.  */
    fprintf (stderr, " inorder fetch width    %16d\n", inorder_fetch_width);
    fprintf (stderr, " inorder issue width    %16d\n", inorder_issue_width);
    fprintf (stderr, " inorder commit width   %16d\n", inorder_commit_width);

    //TODO: L1 cache type
	fprintf (stderr, " l1_cache_size:         %16d\n", l1_cache_size);
	fprintf (stderr, " l1_cache_assoc:        %16d\n", l1_cache_assoc);
	fprintf (stderr, " l1_hit_time:           %16d\n", l1_hit_time);
	fprintf (stderr, " l1_mshrs:              %16d\n", l1_mshrs);
	fprintf (stderr, " l1_replacement_policy: %16d\n", l1_replacement_policy);
	fprintf (stderr, " l1_coherence_policy:   %16d\n", l1_coherence_policy);
	fprintf (stderr, " l1_cache_policy:       %16d\n", l1_cache_policy);
	fprintf (stderr, " l1_lookup_time:        %16d\n", l1_lookup_time);
	fprintf (stderr, " l1_infinite:           %16s\n", l1_infinite == true ? "true" : "false");

    //TODO: L2 cache type
	fprintf (stderr, " l2_cache_size:         %16d\n", l2_cache_size);
	fprintf (stderr, " l2_cache_assoc:        %16d\n", l2_cache_assoc);
	fprintf (stderr, " l2_hit_time:           %16d\n", l2_hit_time);
	fprintf (stderr, " l2_mshrs:              %16d\n", l2_mshrs);
	fprintf (stderr, " l2_replacement_policy: %16d\n", l2_replacement_policy);
	fprintf (stderr, " l2_coherence_policy:   %16d\n", l2_coherence_policy);
	fprintf (stderr, " l2_cache_policy:       %16d\n", l2_cache_policy);
	fprintf (stderr, " l2_lookup_time:        %16d\n", l2_lookup_time);
	fprintf (stderr, " l2_infinite:           %16s\n", l2_infinite == true ? "true" : "false");

    //TODO: L3 cache type
	fprintf (stderr, " l3_cache_size:         %16d\n", l3_cache_size);
	fprintf (stderr, " l3_cache_assoc:        %16d\n", l3_cache_assoc);
	fprintf (stderr, " l3_hit_time:           %16d\n", l3_hit_time);
	fprintf (stderr, " l3_mshrs:              %16d\n", l3_mshrs);
	fprintf (stderr, " l3_replacement_policy: %16d\n", l3_replacement_policy);
	fprintf (stderr, " l3_coherence_policy:   %16d\n", l3_coherence_policy);
	fprintf (stderr, " l3_cache_policy:       %16d\n", l3_cache_policy);
	fprintf (stderr, " l3_lookup_time:        %16d\n", l3_lookup_time);
	fprintf (stderr, " l2_infinite:           %16s\n", l2_infinite == true ? "true" : "false");

	fprintf (stderr, " dir_tiers:             %16d\n", dir_tiers);
    fprintf (stderr, " dir_mode:              %16d\n", dir_mode);
    fprintf (stderr, " dir_addr_per_node_log2:%16d\n", dir_addr_per_node_log2);
    
    fprintf (stderr, " cache_index_swizzle:   0x%14llx\n", (unsigned long long)cache_index_swizzle);
    fprintf (stderr, " dir_home_swizzle:      0x%14llx\n", (unsigned long long)dir_home_swizzle);

    //TODO:  dir_coherence_policy
    fprintf (stderr, " qsets_enabled:         %16s\n", qsets_enabled == true ? "true" : "false");
    fprintf (stderr, " qsets_interval:        %16d\n", qsets_interval);
    fprintf (stderr, " remap_table_size:      %16d\n", remap_table_size);

    //TODO: sel_rep_pred
    //TODO: sel_rep_pred_scope
    //TODO: train_on_loads
    //TODO: train_on_stores
    //TODO: sel_rep_pred_threshold

    fprintf (stderr, " sim_analysis_enabled   %16s\n", sim_analysis_enabled == true ? "true" : "false");
    fprintf (stderr, " ro_tracker_gran        %16d bytes\n", ro_tracker_gran);
    fprintf (stderr, " ro_tracker_entries     %16d bytes\n", ro_tracker_entries);

    /* TODO
		unsigned int pcm_sets;
		unsigned int pcm_assoc;
		unsigned int dram_sets;
		unsigned int dram_assoc;
		unsigned int dram_line_sets;
		unsigned int dram_line_assoc;
		unsigned int pcm_lines;
		unsigned int mem_line_size;

		int dram_hit_time;
		int pcm_hit_time;
		int pcm_rb_hit_time;
		int hdd_hit_time;
	*/

    fprintf (stderr, " network_topology:      %16d\n", network_topology);
	fprintf (stderr, " express_link_len:	  %16d\n", express_link_len);
	fprintf (stderr, " express_link_active:   %16d\n", express_link_active);
	fprintf (stderr, " num_virtual_channels:  %16d\n", num_virtual_channels);
	fprintf (stderr, " buffer_entries_per_vc: %16d\n", buffer_entries_per_vc);
	fprintf (stderr, " debug_addr:            0x%14llx\n", (unsigned long long int) debug_addr);
    fprintf (stderr, " test_addr:             0x%14llx\n", (unsigned long long int) test_addr);

	fprintf (stderr, " sampling_interval:     %lld\n", sampling_interval);
}

void Sim_settings::set_defaults (void)
{
    num_nodes				= 64;
    network_x_dimension		= 8;
    network_y_dimension		= 8;

    num_nhoods              = 0;
    nhood_x_blocking_factor = 0;
    nhood_y_blocking_factor = 0;

    num_mem_ctrls           = 4;

    assert (mem_ctrl_array == NULL);
 
    mem_ctrl_array          = new int [4];
    mem_ctrl_array[0]       = 0;
    mem_ctrl_array[1]       = 4;
    mem_ctrl_array[2]       = 32;
    mem_ctrl_array[3]       = 36;

    heartrate               = (1 << 16);
    net_infinite_bw			= false;
    sharer_forwarding		= true;
    wait_on_inv_acks	    = true;
    livelock_check			= true;
    processor_affinity		= true;
    mem_model_enabled       = false;
    regression_test         = false;
    sesc_rabbit				= 1000000000;
    sesc_nsim               = 0;
    sesc_nsim_per_core      = 10000000;
    sesc_disable_llsc       = false;

    warmup_time				= 0;
    warmup_time_per_core   	= 100000;

    cache_line_size_log2	= 6;
    cache_line_size			= 64;
    
    LSQ_dependence          = true;
    mshrs_per_processor     = 2;
    threads_per_processor   = 1;
    thread_map_policy       = ROUND_ROBIN_MAP;

    simple_issue_width      = 2;

    inorder_fetch_width     = 2;
    inorder_issue_width     = 2;
    inorder_commit_width    = 2;

    l1_cache_type           = CACHE_DATA;
    l1_cache_size           = 32768;
    l1_cache_assoc          = 4;
    l1_hit_time             = 2;
    l1_mshrs                = 2;
    l1_replacement_policy	= RP_LRU;
    l1_coherence_policy		= MESI;
    l1_cache_policy			= CACHE_PRIVATE;
    l1_lookup_time			= 3;
    l1_infinite             = false;
    
    l2_cache_type           = CACHE_DATA;
    l2_cache_size           = 65536;
    l2_cache_assoc          = 8;
    l2_hit_time             = 8;
    l2_replacement_policy	= RP_LRU;
    l2_coherence_policy		= MESI;
    l2_cache_policy			= CACHE_PRIVATE;
    l2_lookup_time			= 3;
    l2_infinite             = false;

    l3_cache_type           = CACHE_TAG;
    l3_cache_size           = 131072;
    l3_cache_assoc          = 16;
    l3_hit_time             = 15;
    l3_replacement_policy	= RP_LRU;
    l3_coherence_policy		= MESI;
    l3_cache_policy			= CACHE_PRIVATE;
    l3_lookup_time			= 3;
    l3_infinite             = false;

    dir_tiers               = 1;
    dir_coherence_policy    = new int[1];
    dir_coherence_policy[0] = MESI;
    dir_mode                = DIR_1L;
    dir_addr_per_node_log2  = 13;

    cache_index_swizzle     = 0x0;
    dir_home_swizzle        = 0x0;

    qsets_enabled           = false;
    qsets_interval          = 1 << 16;

    remap_table_size        = 1024;
    sel_rep_pred            = INVALID_PRED;
    sel_rep_pred_scope      = INVALID_SCOPE;
    train_on_loads          = false;
    train_on_stores         = false;
    sel_rep_pred_threshold  = 0;

    debug = false;

    sim_analysis_enabled    = false;
    ro_tracker_gran         = cache_line_size;
    ro_tracker_entries      = (1 << 14);

    network_topology        = MESH;
	express_link_len		= 4;
	express_link_active		= false;
	num_virtual_channels	= 16;
	buffer_entries_per_vc	= 6;

	sampling_interval	    = 1 << 10;

    debug_addr              = 0x0;
    test_addr               = 0x0;

	data_graph = false;

    report_output           = OUTPUT_FMT_CSV;

    trace_dir               = NULL;
}

