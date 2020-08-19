#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "compress40.h"
#include "except.h"
#include "quantvalues.h"
#include "bitpack.h"
#include "unpack.h"
#include "constants.h"

typedef A2Methods_UArray2 A2;

/*
        compress40.c

        This is the impletemenation of the compress40.c

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/

static int trim_pic(int dim);
static uint32_t pack_codeword(struct package data);
static void print_c(A2 compress, int width,int height, A2Methods_T methods);
static void print_codeword(int col, int row, A2 image, void *elem, void *cl);

/*
Input: file pointer to be compressed
Output: none
Does: trim the picture to get height and width. Makes a A2plain to hold the 
    compressed image data. Uses a loop to look at each pixel block(2x2) and
    calls quant_values module to quantize values, then packs each block
    into a codeword to be stored. then prints the compressed image
*/
void compress40(FILE *input)
{
    A2Methods_T methods = uarray2_methods_plain;
    Pnm_ppm image = Pnm_ppmread(input, methods);
    int height = image->height;
    int width = image->width;
    height = trim_pic(height);
    width = trim_pic(width);
    /*trim pic to get new height and width*/
    A2 compressed = methods->new(width / 2, height / 2, codeword_size);
    uint32_t *temp;
    struct package data;
    Pnm_rgb p1, p2, p3, p4;
    /*loop through every 2x2 block of pixel*/
    for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width / 2; j++) {
                    p1 = methods->at(image->pixels, 2 * j, 2 * i);
                    p2 = methods->at(image->pixels, 2 * j + 1, 2 * i);
                    p3 = methods->at(image->pixels, 2 * j, 2 * i + 1);
                    p4 = methods->at(image->pixels, 2 * j + 1, 2 * i + 1);
                    /*uses quant_values to get their values into a struct */
                    data = quant_values(p1,p2,p3,p4,(double)image->denominator);
                    temp = methods->at(compressed, j ,i);
                    /* packs them here */
                    *temp = pack_codeword(data);
            }
    }
    /*prints it here*/
    print_c(compressed, width, height, methods);
    /*frees it*/
    Pnm_ppmfree(&image);
    methods->free(&compressed);
}

/*
Input: file pointer to be decompressed
Output: none
Does: reads the file in and make sure that is it the proper ppm format
then reads each character one by one and puts them into an array of 4
chars. Then uses unpack module to unpack each arr into four pixels.
Then places them into the plainA2 representing the image. Then uses
ppmwrite to print the image. 
*/
void decompress40(FILE *input)
{
    /* makes sure the header is right format*/
    unsigned height, width;
    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                      &width, &height);
    assert(read == 2);
    int c = getc(input);
    assert(c == '\n');

    int arr[num_bytes];
    struct pixel_block temp;

    A2Methods_T methods = uarray2_methods_plain;
    /*makes new A2 plain array to hold pixels*/
    A2 array = methods->new(width, height, sizeof(struct Pnm_rgb));

    struct Pnm_ppm pixmap = { .width = width, .height = height
                              , .denominator = denominator, .pixels = array
                              , .methods = methods
    };

    Pnm_rgb pix;
    unsigned i = 0;
    unsigned j = 0;
    /*while within bounds, and not end of file, keep reading*/
    while (c != EOF && i < height/2 && j < width/2) {
            for (int k = 0; k < num_bytes; k++) {
                    c = getc(input);
                    assert(c != -1);
                    arr[k] = c;
                    
            }
            temp = unpack(arr);
            pix = methods->at(array, 2 * j, 2 * i);
            *pix = temp.p1;

            pix = methods->at(array, 2 * j + 1, 2 * i);
            *pix = temp.p2;

            pix = methods->at(array, 2 * j, 2 * i + 1);
            *pix = temp.p3;

            pix = methods->at(array, 2 * j + 1, 2 * i + 1);
            *pix = temp.p4;

            j++;
            if (j >= width/2) {
                    j = 0;
                    i++;
            }
    }
    pixmap.pixels = array;

    /*writes then frees the array*/
    Pnm_ppmwrite(stdout, &pixmap);
    methods->free(&array);

}

/*
Input: a struct package consists of a,b,c,d,pb,pr of a 2x2 pixel block
Output: a uint32_t codeword
Does: uses Bitpack_new functions to pack each data into a codeword
*/
static uint32_t pack_codeword(struct package data)
{
        uint32_t codeword = 0;
        codeword = Bitpack_newu(codeword, a_w, a_lsb, data.a);
        codeword = Bitpack_news(codeword, b_w, b_lsb, data.b);
        codeword = Bitpack_news(codeword, c_w, c_lsb, data.c);
        codeword = Bitpack_news(codeword, d_w, d_lsb, data.d);
        codeword = Bitpack_newu(codeword, pb_w, pb_lsb, data.pb);
        codeword = Bitpack_newu(codeword, pr_w, pr_lsb, data.pr);
        return codeword;

}
/*
Input: int dim
Output: trimmed int dim
Does: if dim is not even, trim it by subtracting 1
*/
static int trim_pic(int dim)
{
        if (dim % 2 != 0) {
                return dim-1;
        }
        return dim;
}

/*
Input:A2 compressed image, width, height, and methods
Output:nothing
Does:prints the header then calls a map row major to print each codeword
*/
static void print_c(A2 compress, int width, int height, A2Methods_T methods)
{
    fprintf(stdout, "COMP40 Compressed image format 2\n%u %u", width, height);
    printf("\n");
    methods->map_row_major(compress, print_codeword, &methods);
}

/*
Input: col,row, A2 image, current element, closure
Output: nothing
Does: the apply function used to print codeword. Uses putchar to print
    each codeword.
*/
static void print_codeword(int col, int row, A2 image, void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) image;
        (void) cl;
        uint32_t *codeword = elem;
        for (int i = 3; i >= 0; i--) {
                putchar(Bitpack_getu(*codeword, 8, 8 * i));
        }
}
