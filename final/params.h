#ifndef PARAMS_H
#define PARAMS_H

/*@T
 * \section{System parameters}
 * 
 * The [[sim_param_t]] structure holds the parameters that
 * describe the simulation.  These parameters are filled in
 * by the [[get_params]] function (described later).
 *@c*/
typedef struct sim_param_t {
    char* fname;   /* File name          */
    int   nframes; /* Number of frames   */
    int   npframe; /* Steps per frame    */
    float h;       /* Particle size      */
    float dt;      /* Time step          */
    float rho0;    /* Reference density  */
    float k;       /* Bulk modulus       */
    float mu;      /* Viscosity          */
    float g;       /* Gravity strength   */
} sim_param_t;

int get_params(int argc, char** argv, sim_param_t* params);

/*@q*/
#endif /* PARAMS_H */
