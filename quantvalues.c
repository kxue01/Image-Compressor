#include <stdlib.h>
#include <math.h>
#include <arith40.h>
#include "quantvalues.h"
#include "constants.h"

/*
        quantvalues.c

        This is the impletemenation of the quantvalues.c
        This is the module used to quantize values being passed in

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/

/*
Input: four pixels of a 2x2 pixel block, and denominator of the picture
Output: a struct package containing the a,b,c,d,pb,pr of a 2x2 block
Does: uses formulas provided to calculate y,pb,pr values for each pixel
      and uses them to get a,b,c,d,pb,pr values
*/
struct package quant_values(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3,
    Pnm_rgb p4, double de)
{
    /* gets the follow double values*/
    double y1,pb1,pr1,y2,pb2,pr2,y3,pb3,pr3,y4,pb4,pr4;
    y1 = 0.299 * p1->red/de + 0.587 * p1->green/de + 0.114 * p1->blue/de;
    pb1 = -0.168736 * p1->red/de - 0.331264 * p1->green/de + 0.5 * p1->blue/de;
    pr1 = 0.5 * p1->red/de - 0.418688 * p1->green/de  - 0.081312 * p1->blue/de;

    y2 = 0.299 * p2->red/de + 0.587 * p2->green/de + 0.114 * p2->blue/de;
    pb2 = -0.168736 * p2->red/de - 0.331264 * p2->green/de + 0.5 * p2->blue/de;
    pr2 = 0.5 * p2->red/de - 0.418688 * p2->green/de  - 0.081312 * p2->blue/de;

    y3 = 0.299 * p3->red/de + 0.587 * p3->green/de + 0.114 * p3->blue/de;
    pb3 = -0.168736 * p3->red/de - 0.331264 * p3->green/de + 0.5 * p3->blue/de;
    pr3 = 0.5 * p3->red/de - 0.418688 * p3->green/de  - 0.081312 * p3->blue/de;

    y4 = 0.299 * p4->red/de + 0.587 * p4->green/de + 0.114 * p4->blue/de;
    pb4 = -0.168736 * p4->red/de - 0.331264 * p4->green/de + 0.5 * p4->blue/de;
    pr4 = 0.5 * p4->red/de - 0.418688 * p4->green/de  - 0.081312 * p4->blue/de;
    float pb_avg = (pb1 + pb2 + pb3 + pb4) / 4;
    float pr_avg = (pr1 + pr2 + pr3 + pr4) / 4;

    double a = (y4 + y3 + y2 + y1) / 4.0;
    double b = (y4 + y3 - y2 - y1) / 4.0;
    b = cut(b);
    double c = (y4 - y3 + y2 - y1) / 4.0;
    c = cut(c);
    double d = (y4 - y3 - y2 + y1) / 4.0;
    d = cut(d);

    /*gets the final a,b,c,d,pb,pr values and stores it in a struct package*/
    unsigned a1 = round(a * ((1 << a_w) - 1));
    int b1 = round(b * 50);
    int c1 = round(c * 50);
    int d1 = round(d * 50);
    unsigned pb_idx = Arith40_index_of_chroma(pb_avg);
    unsigned pr_idx = Arith40_index_of_chroma(pr_avg);
    struct package pack = {a1, b1, c1, d1, pb_idx, pr_idx};

    return pack;

}

/*
Input: a double
Output: a double but it is cut to a certain range
Does: if x is less than -.3, return -.3, if it is greater than .3, return .3
      used to quantize values
*/
double cut (double x) {
    if (x > 0.3) {
            return 0.3;
    } else if (x < -0.3) {
            return -0.3;
    }
    return x;
}
