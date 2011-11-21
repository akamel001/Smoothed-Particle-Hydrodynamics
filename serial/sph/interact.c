#include <string.h>
#include <math.h>
#include <assert.h>

#include "params.h"
#include "state.h"
#include "interact.h"

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
    int n = s->n;
    float* restrict rho = s->rho;
    const float* restrict x = s->x;

    float h  = params->h;
    float h2 = h*h;
    float h8 = ( h2*h2 )*( h2*h2 );
    float C  = 4 * s->mass / M_PI / h8;

    memset(rho, 0, n*sizeof(float));
    for (int i = 0; i < n; ++i) {
        rho[i] += 4 * s->mass / M_PI / h2;
        for (int j = i+1; j < n; ++j) {
            float dx = x[2*i+0]-x[2*j+0];
            float dy = x[2*i+1]-x[2*j+1];
            float r2 = dx*dx + dy*dy;
            float z  = h2-r2;
            if (z > 0) {
                float rho_ij = C*z*z*z;
                rho[i] += rho_ij;
                rho[j] += rho_ij;
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
    for (int i = 0; i < n; ++i) {
        a[2*i+0] = 0;
        a[2*i+1] = -g;
    }

    // Constants for interaction term
    float C0 = mass / M_PI / ( (h2)*(h2) );
    float Cp =  15*k;
    float Cv = -40*mu;

    // Now compute interaction forces
    for (int i = 0; i < n; ++i) {
        const float rhoi = rho[i];
        for (int j = i+1; j < n; ++j) {
            float dx = x[2*i+0]-x[2*j+0];
            float dy = x[2*i+1]-x[2*j+1];
            float r2 = dx*dx + dy*dy;
            if (r2 < h2) {
                const float rhoj = rho[j];
                float q = sqrt(r2)/h;
                float u = 1-q;
                float w0 = C0 * u/rhoi/rhoj;
                float wp = w0 * Cp * (rhoi+rhoj-2*rho0) * u/q;
                float wv = w0 * Cv;
                float dvx = v[2*i+0]-v[2*j+0];
                float dvy = v[2*i+1]-v[2*j+1];
                a[2*i+0] += (wp*dx + wv*dvx);
                a[2*i+1] += (wp*dy + wv*dvy);
                a[2*j+0] -= (wp*dx + wv*dvx);
                a[2*j+1] -= (wp*dy + wv*dvy);
            }
        }
    }
}

