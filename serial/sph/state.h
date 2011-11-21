#ifndef STATE_H
#define STATE_H

/*@T
 * \section{System state}
 * 
 * The [[sim_state_t]] structure holds the information for the current
 * state of the system and of the integration algorithm.  The array
 * [[x]] has length $2n$, with [[ x[2*i+0] ]] and [[ x[2*i+1] ]] representing
 * the $x$ and $y$ coordinates of the particle positions.  The layout
 * for [[v]], [[vh]], and [[a]] is similar, while [[rho]] only has one
 * entry per particle.
 * 
 * The [[alloc_state]] and [[free_state]] functions take care of storage
 * for the local simulation state.
 *@c*/
typedef struct sim_state_t {
    int n;                /* Number of particles    */
    float mass;           /* Particle mass          */
    float* restrict rho;  /* Densities              */
    float* restrict x;    /* Positions              */
    float* restrict vh;   /* Velocities (half step) */
    float* restrict v;    /* Velocities (full step) */
    float* restrict a;    /* Acceleration           */
} sim_state_t;

sim_state_t* alloc_state(int n);
void free_state(sim_state_t* s);

/*@q*/
#endif /* STATE_H */
