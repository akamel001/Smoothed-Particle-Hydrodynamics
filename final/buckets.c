#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buckets.h"

int get_bin_pos(sim_state_t* state, sim_param_t* params, int id){
	const float* restrict x = state->x;
	const float h = params->h;	
	const int MAX = state->MAX;
	int ix = (int) (x[2*id+0] / (2*h));
	int iy = (int) (x[2*id+1] / (2*h));
	return (ix+iy*MAX);
}	

void build_bins(sim_state_t* state, sim_param_t* params){

	int n = state->n;
	bin_t* bins = state->bins;
	particle_t* ps = state->ps;
	init_bins(bins, state->bin_size);
	//build bins with proper particle head pointer
	for(int i = 0; i < n; ++i){
		//figure out position for b
		ps[i].x_idx = 2*i+0;
		ps[i].y_idx = 2*i+1;
		ps[i].id = i;
		ps[i].next = NULL;
		put_particle_to_bins(state, params, &ps[i]);
	}
}

void init_bins(bin_t* bins, int bin_size){
	for(int i = 0; i < bin_size; ++i){
		bins[i].id = i;
		bins[i].phead = NULL;
	}
}

particle_t* get_particle_from_bins(sim_state_t* state, sim_param_t* params, int particle_id){
	bin_t* bins = state->bins;
	int bidx = get_bin_pos(state, params, particle_id);
	particle_t *p = bins[bidx].phead;

	if (p == NULL) return NULL;
	while(p->id != particle_id && p->next != NULL){
		p = p->next;
	}
	return p;
}


void put_particle_to_bins(sim_state_t* state, sim_param_t* params, particle_t* p){
	bin_t* bins = state->bins;
	int bidx = get_bin_pos(state, params, p->id);
	particle_t* phead = bins[bidx].phead;
	p->next = phead;
	bins[bidx].phead = p;
}

void neighbors3(sim_state_t* state, sim_param_t* param, int bidx, particle_t* p, int* nns4) {
	int MAX = state->MAX;
	const int bin_size = state->bin_size;

	nns4[0] = bidx;
	nns4[1] = bidx + MAX - 1;
	nns4[2] = bidx + MAX;
	nns4[3] = bidx + MAX + 1;
	nns4[4] = bidx - 1;
	nns4[5] = bidx + 1;
	nns4[6] = bidx - MAX - 1;
	nns4[7] = bidx - MAX;
	nns4[8] = bidx - MAX + 1;

	for (int i = 0; i < 9; ++i) {
		if (nns4[i] < 0 || nns4[i] >= bin_size) nns4[i] = -1;
	}

}

particle_t* remove_particle_from_bins(sim_state_t* state, int particle_id, int bidx){
	bin_t* bins = state->bins;
	if (particle_id < 0) return NULL;
	particle_t* p_cur = bins[bidx].phead;
	particle_t* p_prev = NULL;
	if (p_cur == NULL) return NULL;
	while(p_cur->id != particle_id && p_cur->next != NULL){
		p_prev = p_cur;
		p_cur = p_cur->next;
	}
	if (p_cur->id != particle_id) return NULL;
	if (p_prev == NULL) {
		bins[bidx].phead = p_cur->next;
		p_cur->next = NULL;
		return p_cur;
	}
	p_prev->next = p_cur->next;
	p_cur->next = NULL;
	return p_cur;
}

int update_bins(sim_state_t* state, sim_param_t* params){
	bin_t* bins = state->bins;
	const int bin_size = state->bin_size;
	particle_t* p_change;
	for (int i = 0; i < bin_size; i++) {
		particle_t* p = bins[i].phead;
		while(p != NULL) {
			int bidx = get_bin_pos(state, params, p->id);
			if (bidx != i) {
				p_change = p;
				p = p->next;
				p_change = remove_particle_from_bins(state, p_change->id, i);
				put_particle_to_bins(state, params, p_change);
			} else {
				p = p->next;
			}
		}
	}
	return 1;
}

int optimize_bins(sim_state_t* state, sim_param_t* params, int first_time){
	bin_t* bins = state->bins;
	int n = state->n;
	int idx = 0;
	particle_t* ps_new = (particle_t*) calloc(n, sizeof(particle_t));

	int bin_size = state->bin_size;
	for (int b = 0; b < bin_size; b++) {
		particle_t* p = bins[b].phead;

		particle_t* prev = NULL;

		if (p ==  NULL) bins[b].phead = NULL;
		else bins[b].phead = &ps_new[idx];

		while (p != NULL) {
			deep_copy(p, &ps_new[idx]);
			p = p->next;
			if (prev != NULL)
				ps_new[idx-1].next = &ps_new[idx];
			prev = p;
			idx = idx + 1;
		}
	}
	free(state->ps);
	state->ps = ps_new;
	return 1;
}

void deep_copy(particle_t* p_old, particle_t* p_new) {
	p_new->x_idx = p_old->x_idx;
	p_new->y_idx = p_old->y_idx;
	p_new->id = p_old->id;
	p_new->next = NULL;
}
