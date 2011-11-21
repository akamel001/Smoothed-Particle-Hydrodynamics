#include <stdlib.h>
#include "state.h"

sim_state_t* alloc_state(int n, float h)
{
    int MAX =  (int) (1 / (2 * h));
    sim_state_t* s = (sim_state_t*) calloc(1, sizeof(sim_state_t));
    s->n   =  n;
    s->MAX =  MAX;
    s->bin_size = MAX * MAX;
    s->ps = (particle_t*) calloc(n, sizeof(particle_t));
    s->bins = (bin_t*) calloc(MAX*MAX, sizeof(bin_t));
    s->rho =  (float*) calloc(  n, sizeof(float));
    s->x =    (float*) calloc(2*n, sizeof(float));
    s->vh =   (float*) calloc(2*n, sizeof(float));
    s->v =    (float*) calloc(2*n, sizeof(float));
    s->a =    (float*) calloc(2*n, sizeof(float));
    return s;
}

void free_state(sim_state_t* s)
{
    free(s->a);
    free(s->v);
    free(s->vh);
    free(s->x);
    free(s->rho);
    free(s->ps);
    free(s->bins);
    free(s);
}

