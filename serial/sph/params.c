#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "params.h"


/*@T
 * \section{Option processing}
 * 
 * The [[print_usage]] command documents the options to the [[nbody]]
 * driver program, and [[default_params]] sets the default parameter
 * values.  You may want to add your own options to control
 * other aspects of the program.  This is about as many options as
 * I would care to handle at the command line --- maybe more!  Usually,
 * I would start using a second language for configuration (e.g. Lua)
 * to handle anything more than this.
 *@c*/
static void default_params(sim_param_t* params)
{
    params->fname   = "run.out";
    params->nframes = 400;
    params->npframe = 100;
    params->dt      = 1e-4;
    params->h       = 1e-2;
    params->rho0    = 1000;
    params->k       = 1e3;
    params->mu      = 0.1;
    params->g       = 9.8;
}

static void print_usage()
{
    sim_param_t param;
    default_params(&param);
    fprintf(stderr, 
            "nbody\n"
            "\t-h: print this message\n"
            "\t-o: output file name (%s)\n"
            "\t-F: number of frames (%d)\n"
            "\t-f: steps per frame (%d)\n"
            "\t-t: time step (%e)\n"
            "\t-s: particle size (%e)\n"
            "\t-d: reference density (%g)\n"
            "\t-k: bulk modulus (%g)\n"
            "\t-v: dynamic viscosity (%g)\n"
            "\t-g: gravitational strength (%g)\n",
            param.fname, param.nframes, param.npframe,
            param.dt, param.h, param.rho0,
            param.k, param.mu, param.g);
}

/*@T
 *
 * The [[get_params]] function uses the [[getopt]] package
 * to handle the actual argument processing.  Note that
 * [[getopt]] is {\em not} thread-safe!  You will need to
 * do some synchronization if you want to use this function
 * safely with threaded code.
 *@c*/
int get_params(int argc, char** argv, sim_param_t* params)
{
    extern char* optarg;
    const char* optstring = "ho:F:f:t:s:d:k:v:g:";
    int c;

    #define get_int_arg(c, field) \
        case c: params->field = atoi(optarg); break
    #define get_flt_arg(c, field) \
        case c: params->field = (float) atof(optarg); break

    default_params(params);
    while ((c = getopt(argc, argv, optstring)) != -1) {
        switch (c) {
        case 'h': 
            print_usage(); 
            return -1;
        case 'o':
            strcpy(params->fname = malloc(strlen(optarg)+1), optarg);
            break;
        get_int_arg('F', nframes);
        get_int_arg('f', npframe);
        get_flt_arg('t', dt);
        get_flt_arg('s', h);
        get_flt_arg('d', rho0);
        get_flt_arg('k', k);
        get_flt_arg('v', mu);
        get_flt_arg('g', g);
        default:
            fprintf(stderr, "Unknown option\n");
            return -1;
        }
    }
    return 0;
}
