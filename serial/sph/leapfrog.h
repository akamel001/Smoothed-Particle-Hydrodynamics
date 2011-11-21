#ifndef LEAPFROG_H
#define LEAPFROG_H

#include "state.h"

void leapfrog_start(sim_state_t* s, double dt);
void leapfrog_step(sim_state_t* s, double dt);

#endif /* LEAPFROG_H */
