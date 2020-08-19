#include "unpack.h"
#include "quantvalues.h"
#include "constants.h"

/*
        unpack.c

        This is the impletemenation of the unpack.c
        This is the module where we take a codeword and unpack it into a 2x2
            pixel block.

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/


/*
Input: int array comprised of four ints, each representing a char from codeword
Output: struct pixel_block containing pnm_rgb of a 2x2 block of pixels
Does: gets codeword from array, uses formulas provided to get back the
      correct a,b,c,d,pb,pr values, and gets four pixels back
*/
struct pixel_block unpack(int *arr)
{
        uint32_t codeword = get_codeword(arr);
        struct pixel_block pix;
        uint32_t a_prelim = Bitpack_getu(codeword, a_w, a_lsb);
        double a = (double)a_prelim / ((1 << a_w) - 1);

        int32_t b_prelim = Bitpack_gets(codeword, b_w, b_lsb);
        double b = (double)b_prelim / 50;

        int32_t c_prelim = Bitpack_gets(codeword, c_w, c_lsb);
        double c = (double)c_prelim / 50;

        int32_t d_prelim = Bitpack_gets(codeword, d_w, d_lsb);
        double d = (double)d_prelim / 50;

        uint32_t pb_ind = Bitpack_getu(codeword, pb_w, pb_lsb);

        uint32_t pr_ind = Bitpack_getu(codeword, pr_w, pr_lsb);

        pix.p1 = get_rgb((a - b - c + d), pb_ind, pr_ind);
        pix.p2 = get_rgb((a - b + c - d), pb_ind, pr_ind);
        pix.p3 = get_rgb((a + b - c - d), pb_ind, pr_ind);
        pix.p4 = get_rgb((a + b + c + d), pb_ind, pr_ind);
        return pix;
}

/*
Input: int array comprised of four ints, each representing a char from codeword
Output: uint32_t codeword
Does: shifts each index of the array by 8, to get codeword in big endian order
*/
uint32_t get_codeword(int *arr)
{
        uint32_t codeword = 0;
        codeword = codeword | (arr[0] << 24);
        codeword = codeword | (arr[1] << 16);
        codeword = codeword | (arr[2] << 8);
        codeword = codeword | arr[3];
        return codeword;
}

/*
Input: y, pb index, and pr index
Output: pnm_rgb pixel from the calcualted r,g,b values
Does: uses formulas to calculate r,g,b, valuse and makes them a pixel
*/
struct Pnm_rgb get_rgb(double y, int pb_ind, int pr_ind)
{
        struct Pnm_rgb pixel;
        float pb = Arith40_chroma_of_index(pb_ind);
        float pr = Arith40_chroma_of_index(pr_ind);

        int red = round((1.0 * y + 0.0 * pb + 1.402 * pr) * denominator);
        pixel.red = cut_color(red);

        int green = round((1.0 * y - 0.344136 * pb - 0.714136 * pr) *
                          denominator);
        pixel.green = cut_color(green);

        int blue = round((1.0 * y + 1.772 * pb + 0.0 * pr) * denominator);
        pixel.blue = cut_color(blue);

        return pixel;
}

/*
Input: int color of r,g,b
Output: unsigned int
Does: if an r,g,b, value is less than 0, makes it into 0. If it is more than
      1, makes it into 1.
*/
unsigned cut_color(int color) {
        if (color > denominator) {
                return denominator;
        } else if (color < 0) {
                return 0;
        }
        return (unsigned)color;
}
