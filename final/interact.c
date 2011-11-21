#include <string.h>
#include <math.h>
#include <assert.h>
//#include <omp.h>
#include <stdlib.h>

#include "params.h"
#include "state.h"
#include "interact.h"
#include <stdio.h>
#include "buckets.h"

/*@q
 * ====================================================================
 */

/*@T
 * \subsection{Density computations}
 * 
 * The formula for density is
 * \[
 *   \rho_i = \frac{4m}{\pi h^8} \sum_{j \in N_i} (h^2 - r^2)^3.
 * \]
 * We search for neighbors of node $i$ by checking every particle,
 * which is not very efficient.  We do at least take advange of
 * the symmetry of the update ($i$ contributes to $j$ in the same
 * way that $j$ contributes to $i$).
 *@c*/

void compute_density(sim_state_t* s, sim_param_t* params)
{
    const int n = s->n;
    float* restrict rho = s->rho;
    const float* restrict x = s->x;
    const float h  = params->h;
    float h2 = h*h;
    float h8 = ( h2*h2 )*( h2*h2 );
    float C  = 4 * s->mass / M_PI / h8;
    const int BIN_SIZE = s->bin_size;

    memset(rho, 0, n*sizeof(float));
    bin_t* bins = s->bins;

#pragma omp parallel shared(bins, rho, h2, C, params, s, x)
	 {
		 int nns4[9];
#pragma omp for schedule(static)
		 for (int b = 0; b < BIN_SIZE; b++) {
			 particle_t* pi = bins[b].phead;
			 while (pi != NULL) {
				 rho[pi->id] += 4 * s->mass / M_PI / h2;
				 neighbors3(s, params, b, pi, nns4);
				 for (int i = 0; i < 9; ++i) {
					 int bj = nns4[i];
					 if (bj == -1) continue;
					 particle_t* pj = bins[bj].phead;
					 while (pj != NULL) {
						 if (pj->id != pi->id) {
							 float dx = x[pi->x_idx]-x[pj->x_idx];
							 float dy = x[pi->y_idx]-x[pj->y_idx];
							 float r2 = dx*dx + dy*dy;
							 float z  = h2-r2;
							 if (z > 0) {
								 float rho_ij = C*z*z*z;
								 rho[pi->id] += rho_ij;
							 }
						 }
						 pj = pj->next;
					 }
				 }
				 pi = pi->next;
			 }
		 }
	 }
}

/*@T
 * \subsection{Computing forces}
 * 
 * The acceleration is computed by the rule
 * \[
 *   \bfa_i = \frac{1}{\rho_i} \sum_{j \in N_i} 
 *     \bff_{ij}^{\mathrm{interact}} + \bfg,
 * \]
 * where the pair interaction formula is as previously described.
 * Like [[compute_density]], the [[compute_accel]] routine takes
 * advantage of the symmetry of the interaction forces
 * ($\bff_{ij}^{\mathrm{interact}} = -\bff_{ji}^{\mathrm{interact}}$)
 * but it does a very expensive brute force search for neighbors.
 *@c*/

void compute_accel(sim_state_t* state, sim_param_t* params)
{
    // Unpack basic parameters
    const float h    = params->h;
    const float rho0 = params->rho0;
    const float k    = params->k;
    const float mu   = params->mu;
    const float g    = params->g;
    const float mass = state->mass;
    const float h2   = h*h;
    
    // Unpack system state
    const float* restrict rho = state->rho;
    const float* restrict x   = state->x;
    const float* restrict v   = state->v;
    float* restrict a         = state->a;
    int n = state->n;
    // Compute density and color
    compute_density(state, params);

    // Start with gravity and surface forces
    for (int j = 0; j < n; ++j) {
        a[2*j+0] = 0;
        a[2*j+1] = -g;
    }

	 // Constants for interaction term
	 float C0 = mass / M_PI / ( (h2)*(h2) );
	 float Cp =  15*k;
	 float Cv = -40*mu;
	 // Now compute interaction forces
	 int BIN_SIZE = state->bin_size;

	 bin_t* bins = state->bins;
#pragma omp parallel shared(bins, BIN_SIZE, n, rho, x, v, a, state, params)
	 {
		 int nns4[9];
#pragma omp for schedule(static)
		 for (int b = 0; b < BIN_SIZE; ++b) {
			 particle_t* pi = bins[b].phead;
			 while (pi != NULL) {
				 const float rhoi = rho[pi->id];
				 neighbors3(state, params, b, pi, nns4);
				 for (int i = 0; i < 9; ++i) {
					 int bj = nns4[i];
					 if (bj == -1) continue;
					 particle_t* pj = bins[bj].phead;
					 while (pj != NULL) {
						 if (pj->id == pi->id) {pj = pj->next; continue;}
						 float dx = x[pi->x_idx]-x[pj->x_idx];
						 float dy = x[pi->y_idx]-x[pj->y_idx];
						 float r2 = dx*dx + dy*dy;
						 if (r2 < h2) {
							 const float rhoj = rho[pj->id];
							 float q = sqrt(r2)/h;
							 float u = 1-q;
							 float w0 = C0 * u/rhoi/rhoj;
							 float wp = w0 * Cp * (rhoi+rhoj-2*rho0) * u/q;
							 float wv = w0 * Cv;
							 float dvx = v[pi->x_idx]-v[pj->x_idx];
							 float dvy = v[pi->y_idx]-v[pj->y_idx];
							 a[pi->x_idx] += (wp*dx + wv*dvx);
							 a[pi->y_idx] += (wp*dy + wv*dvy);
						 }
						 pj = pj->next;
					 }
				 }
				 pi = pi->next;
			 }
		 }
	 }
}
