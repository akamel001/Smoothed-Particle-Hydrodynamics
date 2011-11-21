#include <stdlib.h>
#include "state.h"

sim_state_t* alloc_state(int n)
{
    sim_state_t* s = (sim_state_t*) calloc(1, sizeof(sim_state_t));
    s->n   =  n;
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
    free(s);
}
