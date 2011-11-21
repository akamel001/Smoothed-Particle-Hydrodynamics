#include <stdio.h>
#include "io.h"

#define VERSION_TAG "SPHView00 "


void write_header(FILE* fp, int n)
{
    fprintf(fp, "%s%d 1\n", VERSION_TAG, n);
}


void write_frame_data(FILE* fp, int n, float* x, int* c)
{
    for (int i = 0; i < n; ++i) {
        float xi = *x++;
        float yi = *x++;
        int   ci = c ? *c++ : 0;
        fprintf(fp, "%e %e %d\n", xi, yi, ci);
    }
}
