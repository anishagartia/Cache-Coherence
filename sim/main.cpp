
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "sim.h"
#include "settings.h"

Sim_settings settings;

Simulator *Sim;

extern char *optarg;
extern int optind, optopt;

void usage (void)
{
    fprintf (stderr, "Usage:\n");
    fprintf (stderr, "\t-p <protocol> (choices MI, MSI, MESI)\n");
    fprintf (stderr, "\t-t <trace directory>\n\n");
}

int main (int argc, char *argv[])
{
    int num_nodes = 0;
    char *trace_dir = NULL;
    char *protocol = NULL;
    FILE *config_file = NULL;
    char config_path[1000];
    bool debug = false;

    /** Parse command line arguments.  */
    int c;

    while ((c = getopt(argc, argv, "hP:p:t:")) != -1)
    {
        switch(c)
        {
        case 'h':
            usage ();
            exit (0);
            break;

        case 'p':
            protocol = strdup (optarg);
            break;

        case 't':
            trace_dir = strdup (optarg);
            break;

        default:
            fprintf (stderr, "Invalid command line arguments - %c", c);
            usage ();
            exit (-1);
        }
    }

    sprintf(config_path,"%s/config",trace_dir);
    config_file = fopen (config_path,"r");
    if (fscanf(config_file,"%d\n",&num_nodes) != 1)
    {
    	fatal_error("Config File should contain number of traces\n");
    }

    if (num_nodes == 0)
        fatal_error ("Error: number of processors is zero.\n");

    if (protocol == NULL)
        fatal_error ("Error: invalid protocol specified.\n");

    if (trace_dir == NULL)
        fatal_error ("Error: trace file directory not defined!\n");



    /** Init settings.  */
    settings.set_defaults ();
    settings.num_nodes = num_nodes;
    settings.trace_dir = trace_dir;

    if (!strcmp(protocol,"MI"))
    {
    	settings.protocol = MI_PRO;
    }
    else if (!strcmp(protocol,"MSI"))
    {
    	settings.protocol = MSI_PRO;
    }
    else if (!strcmp(protocol,"MESI"))
    {
    	settings.protocol = MESI_PRO;
    }
    else if (!strcmp(protocol,"MOSI"))
    {
    	settings.protocol = MOSI_PRO;
    }
    else if (!strcmp(protocol,"MOESI"))
    {
    	settings.protocol = MOESI_PRO;
    }
    else if (!strcmp(protocol,"MOESIF"))
    {
    	settings.protocol = MOESIF_PRO;
    }
    else
    {
    	fatal_error ("Error: invalid protocol specified.\n");
    }

    //TODO: Add MI, MSI, MESI to config; Hardcoded for MI now    

    /** Build simulator.  */
    Sim = new Simulator ();
    Sim->run ();
}
