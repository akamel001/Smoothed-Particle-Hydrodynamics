#ifndef INTERACT_H
#define INTERACT_H

#include "params.h"
#include "state.h"

void compute_density(sim_state_t* s, sim_param_t* params);
void compute_accel(sim_state_t* state, sim_param_t* params);

#endif /* INTERACT_H */
