#include "params.h"
#include "state.h"

int get_bin_pos(sim_state_t* state, sim_param_t* params, int id);

void build_bins(sim_state_t* state, sim_param_t* params);

void init_bins(bin_t* bins, int bin_size);

particle_t* get_particle_from_bins(sim_state_t* state, sim_param_t* params, int particle_id);

void put_particle_to_bins(sim_state_t* state, sim_param_t* params, particle_t* p);

particle_t* remove_particle_from_bins(sim_state_t* state, int particle_id, int bidx);

int update_bins(sim_state_t* state, sim_param_t* params);

int optimize_bins(sim_state_t* state, sim_param_t* params, int first_time);

void deep_copy(particle_t* p_old, particle_t* p_new);

void neighbors3(sim_state_t* state, sim_param_t* param, int bidx, particle_t* p, int* nns4);

