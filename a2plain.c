#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/*
                a2plain.c

        This is the impletemenation of the a2plain

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)
*/

/* Does: makes a new UArray2_T with the specified col, row, and size.
 Expects: a positive non-zero col, row, and size.
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
  return UArray2_new(width, height, size);
}

/* Does: makes a new UArray2_T with the specified col, row, and size.
Expects: a positive non-zero col, row, and size. Blocksize is not used
*/
static A2Methods_UArray2 new_with_blocksize(int width,
  int height, int size, int blocksize)
{
  (void) blocksize;
  return UArray2_new(width, height, size);
}

/* Does: Frees the a given UArray2
   Expects: a non-null pointer to a UArray2
   No Output
*/
static void a2free(A2Methods_UArray2 *array2p)
{
  UArray2_free((UArray2_T *) array2p);
}

/* Does: Returns the width of given UArray2
 Expects: a non-null pointer to a UArray2.
 Outputs a Int greater than 0
*/
static int width(A2Methods_UArray2 array2)
{
  return UArray2_width(array2);
}

/* Does: Returns the height of given UArray2
 Expects: a non-null pointer to a UArray2.
 Outputs a Int greater than 0
*/
static int height(A2Methods_UArray2 array2)
{
  return UArray2_height(array2);
}

/* Does: Returns the size of given UArray2
 Expects: a non-null pointer to a UArray2.
 Outputs a Int greater than 0
*/
static int size(A2Methods_UArray2 array2)
{
  return UArray2_size(array2);
}

/* Does: Returns the blocksize of given UArray2
 Expects: a non-null pointer to a UArray2.
 Outputs 1, since each cell of UArray2 is a block
*/
static int blocksize(A2Methods_UArray2 array2)
{
  (void) array2;
  return 1;
}

/*Does: Returns a void* pointer to the element at given row and col
Expects: a non-null pointer to a UArray2, and non-zero postive row and col
         Outputs a non-null void* AKA A2Methods_Object
*/
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
  return UArray2_at(array2, i, j);
}


/*
Description: applies an apply function to the Uarray2_t
    pointed to by uarray2 by traversing over an entire
    row of the uarray2 before moving on to the next row of the arr
Input: A pointer to a UArray2_T, an apply function of type
    void, a pointer to a closure
Output: nothing
*/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*
Description: applies an apply function to the Uarray2_t
    pointed to by uarray2 by traversing over an entire
    col of the uarray2 before moving on to the next col of the arr
Input: A pointer to a UArray2_T, an apply function of type
    void, a pointer to a closure
Output: nothing
*/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}


struct small_closure {
  A2Methods_smallapplyfun *apply; 
  void                    *cl;
};

/* Small apply function - a part of starter code*/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)uarray2;
  cl->apply(elem, cl->cl);
}
/* Small map_row_major function - a part of starter code*/
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_row_major(a2, apply_small, &mycl);
}

/* Small map_col_major function - a part of starter code*/
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
  new,
  new_with_blocksize,
  a2free,
  width,
  height,
  size,
  blocksize,
  at,
  map_row_major,
  map_col_major,
  NULL, /* map_block_major */
  map_row_major, /* map_default*/
  small_map_row_major,
  small_map_col_major,
  NULL, /*small_map_block_major*/
  small_map_row_major, /* small_map_default */
};

/* finally the payoff: here is the exported pointer to the struct */

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
