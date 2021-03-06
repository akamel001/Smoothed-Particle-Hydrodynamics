#include "timing.h"
#include <time.h>

/* If the clock macro is defined, we use the POSIX clock_gettime,
 * and CLOCK defines which timer should be used.  Otherwise, we use
 * the system clock() command.
 */


#ifdef CLOCK
static struct timespec watches[NWATCHES];
#elseif GTD
static struct timeval watches[NWATCHES];
#else
static clock_t watches[NWATCHES];
#endif


void tic(int watch)
{
#ifdef CLOCK
    clock_gettime(CLOCK, watches+watch);
#elseif GTD
    gettimeofday(watches+watch, NULL);
#else
    watches[watch] = clock();
#endif
}


double toc(int watch)
{
    double elapsed;
#ifdef CLOCK
    struct timespec now;
    clock_gettime(CLOCK, &now);
    elapsed = now.tv_nsec - (double) watches[watch].tv_nsec;
    elapsed *= 1.0E-9;
    elapsed += now.tv_sec - (double) watches[watch].tv_sec;
#elseif GTD
    struct timeval now;
    gettimeofday(&now, NULL);
    elapsed = now.tv_usec = (double) watches[watch].tv_sec;
    elapsed *= 1.0e-6;
    elapsed += now.tv_sec = (double) watches[watch].tv_sec;
#else
    clock_t now = clock();
    elapsed = (double) (now-watches[watch])/CLOCKS_PER_SEC;
#endif    
    return elapsed;
}
