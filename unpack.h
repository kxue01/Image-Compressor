#ifndef UNPACK_INCLUDED
#define UNPACK_INCLUDED

#include <pnm.h>
#include <bitpack.h>
#include <math.h>
#include <arith40.h>


/*
        unpack.h

        This is the header file of the unpack.c
        This is the module where we take a codeword and unpack it into a 2x2 
            pixel block.

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/

struct pixel_block {
        struct Pnm_rgb p1;
        struct Pnm_rgb p2;
        struct Pnm_rgb p3;
        struct Pnm_rgb p4;
};

struct pixel_block unpack(int *arr);
uint32_t get_codeword(int *arr);
struct Pnm_rgb get_rgb(double y, int pb_ind, int pr_ind);
unsigned cut_color(int color);

#endif
