/* Copyright (C) 2012 Papavasileiou Dimitris
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef enum {
    ARRAY_TDOUBLE,
    ARRAY_TFLOAT,
    ARRAY_TULONG,
    ARRAY_TLONG,
    ARRAY_TUINT,
    ARRAY_TINT,
    ARRAY_TUSHORT,
    ARRAY_TSHORT,
    ARRAY_TUCHAR,
    ARRAY_TCHAR
} array_Type;

typedef struct {
    /* Placing the union first allows one to simply cast a pointer to
     * an array struct to a pointer to a suitable numeric type and use
     * it as a normal C array. */
    
    union values {
	void *any;
	double *doubles;
	float *floats;
	unsigned long *ulongs;
	signed long *longs;
	unsigned int *uints;
	signed int *ints;
	unsigned short *ushorts;
	signed short *shorts;
	unsigned char *uchars;
	signed char *chars;
    } values;

    array_Type type;

    enum {
	FREE_NOTHING,
	FREE_SIZE,
	FREE_VALUES,
	FREE_BOTH
    } free;
    
    int rank, *size;
} array_Array;

int luaopen_array_core (lua_State *L);

array_Array *array_testarray (lua_State *L, int index);
array_Array *array_checkarray (lua_State *L, int index);
array_Array *array_checkcompatible (lua_State *L, int index,
				    array_Type type, int rank, ...);
array_Array *array_testcompatible (lua_State *L, int index,
				   array_Type type, int rank, ...);
void array_copy (lua_State *L, int index);
void array_set (lua_State *L, int index, lua_Number c);
void array_cast (lua_State *L, int index, int rank, ...);
void array_toarray (lua_State *L, int index, array_Type type, int rank, ...);
void array_slicev (lua_State *L, int index, int *slices);
void array_castv (lua_State *L, int index, int rank, int *size);
void array_toarrayv (lua_State *L, int index, array_Type type, int rank, int *size);
void array_createarrayv (lua_State *L, array_Type type, void *values,
			 int rank, int *size);
void array_createarray (lua_State *L, array_Type type, void *values,
			int rank, ...);
void array_pusharray (lua_State *L, array_Array *array);

#endif
