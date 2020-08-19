#ifndef QUANT_INCLUDED
#define QUANT_INCLUDED

#include <stdlib.h>
#include <math.h>
#include <arith40.h>
#include <pnm.h>

/*
        quantvalues.h

        This is the header of the quantvalues.h
        This is the module used to quantize values being passed in 

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/

struct package {
    unsigned a;
    int b;
    int c;
    int d;
    unsigned pb;
    unsigned pr;
};

struct package quant_values(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, Pnm_rgb p4,
 double de);
double cut(double x);

#endif
