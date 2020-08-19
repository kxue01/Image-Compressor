#include <bitpack.h>
#include "except.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "assert.h"

/*
        bitpack.c

        This is the impletemenation of the bitpack.c

        Authors: Kenneth Xue (kxue01)
                Sam Chan(schan07)

*/


static bool check_overflow(unsigned w, unsigned lsb);

Except_T Bitpack_Overflow = { "Overflow packing bits" };


const unsigned size = 64;


/*
Input: unsigned int, and width
Output: true if n fits inside width number of bits, false if not
Does: gets 2^width . If that is bigger than n, then return true.
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width > 64) {
            return false;
        }
        if (width == 64) {
            return true;
        }
        uint64_t temp = 1 << width;
        if (n < temp) {
                return true;
        }
        return false;
}

/*
Input: signed int, and width
Output: true if n fits inside width number of bits, false if not
Does: gets 2^width - 1 and -2^width. If n is within these bounds
      return true, else false
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{  
        if (width > 64) {
            return false;
        }
        if (width == 64) {
            return true;
        }
        int64_t temp = 1 << (width - 1);
        if (n < (-1 * temp) || n > (temp - 1)) {
                return false;
        }
        return true;
}

/*
Input: unsigned word, unsigned width, and unsigned lsb
Output: the data from given width and lsb, unsigned
Does: makes a mask that looks only at the data field from inputs. 
then ands the mask and the word to get the data, then shift it to the right
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        if (check_overflow(width, lsb)) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t mask1 = ~((uint64_t)0);
        if (lsb + width >= size) {
                mask1 = 0;
        } else {
                mask1 = mask1 << (lsb + width);
        }
        uint64_t mask2 = ~((uint64_t)0);
        if (lsb == 0) {
                mask2 = ((uint64_t)0);
        }
        else {
                mask2 = mask2  >> (size - lsb);
        }
        mask1 = mask1 | mask2;
        mask1 = ~mask1; /*mask is made*/
        uint64_t data = mask1 & word;
        data = data >> lsb; /*shifts data to the most right position*/
        return data;

}

/*
Input: unsigned word, unsigned width, and unsigned lsb
Output: the data from given width and lsb, signed
Does: makes a mask that looks only at the data field from inputs. 
then ands the mask and the word to get the data, then shift it to the right
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        if (check_overflow(width, lsb)) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t mask1 = ~((uint64_t)0);
        if (lsb + width >= size) {
                mask1 = ((uint64_t)0);
        } else {
                mask1 = mask1 << (lsb + width);
        }
        uint64_t mask2 = ~((uint64_t)0);
        if (lsb == 0) {
                mask2 = ((uint64_t)0);
        }
        else {
                mask2 = mask2  >> (size - lsb);
        }
        uint64_t mask = mask1 | mask2;
        mask = ~mask; /*mask is made*/
        int64_t data = mask & word;
        if (lsb + width - 1 >= size) {
                word = ((uint64_t)0);
        } else {
                word = word >> (lsb + width - 1);
        }
        word = word & ((uint64_t)1); /*gets if data is positive or negative*/
        if (word == 1) { /* if negative, apply mask to make it negative*/
                data = data | mask1;
        }
        data = data >> lsb; /*shifts data to the most right position*/
        return data;
}

/*
Input: unsigned word, unsigned width, and unsigned lsb, unsigned value
Output: a new word with its field updated with value
Does: makes a mask of that isolates that value field, then zeros out the
      field. then shift value to the right lsb and & the word and value
      to update the value field. 
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
    uint64_t value)
{
        if (check_overflow(width, lsb)) {
                RAISE(Bitpack_Overflow);
        }
        if (!Bitpack_fitsu(value, width)) {
            assert(0);
        }
        uint64_t mask1 = ~((uint64_t)0);
        if (lsb + width >= size) {
                mask1 = ((uint64_t)0);
        } else {
                mask1 = mask1 << (lsb + width);
        }
        uint64_t mask2 = ~((uint64_t)0);
        if (lsb == 0) {
                mask2 = ((uint64_t)0);
        }
        else {
                mask2 = mask2  >> (size - lsb);
        }
        mask1 = mask1 | mask2; /*mask is made*/
        uint64_t data = mask1 & word; /*zeros the field*/
        value = value << lsb;
        data = data | value;
        return data;
}

/*
Input: unsigned word, unsigned width, and unsigned lsb, signed value
Output: a new word with its field updated with value
Does: makes a mask of that isolates that value field, then zeros out the
      field. then shift value to the right lsb and & the word and value
      to update the value field. 
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
    int64_t value)
{
        if (check_overflow(width, lsb)) {
                RAISE(Bitpack_Overflow);
        }
        if (!Bitpack_fitss(value, width)) {
            assert(0);
        }
        uint64_t mask1 = ~((uint64_t)0);
        if (lsb + width >= size) {
                mask1 = ((uint64_t)0);
        } else {
                mask1 = mask1 << (lsb + width);
        }
        uint64_t mask2 = ~((uint64_t)0);
        if (lsb == 0) {
                mask2 = ((uint64_t)0);
        }
        else {
                mask2 = mask2  >> (size - lsb);
        }
        mask1 = mask1 | mask2; /*mask is made*/
        uint64_t data = mask1 & word;
        value = value << lsb;
        value = (~mask1) & value; /*takes into account if it's negative*/
        data = data | value;
        return data;
}

/*
Input: unsigned width and unsigned lsb
Output: true of w+lsb overflows (greater than size)
Does: checks if w + lsb > 64. Returns true if it does
*/
static bool check_overflow(unsigned w, unsigned lsb)
{
        if (w + lsb > 64) {
                return true;
        }
        return false;
}
