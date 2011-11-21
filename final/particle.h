typedef struct particle_t {
	int  id;
	int  x_idx;
	int  y_idx;
	float dens;

	struct particle_t* next;

} particle_t;

typedef struct bin_t {
	int  id;
	particle_t* phead;

} bin_t;
