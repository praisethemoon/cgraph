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

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "array.h"

static int create (lua_State *L)
{
    int i, j;

    for (i = 0 ; lua_type (L, i + 1) == LUA_TNUMBER ; i += 1);

    if (lua_gettop (L) == i) {
	lua_pushnil(L);
    }

    {
	int size[i];

	for (j = 0 ; j < i ; j += 1) {
	    size[j] = lua_tonumber (L, j + 1);
	}
	
	array_toarrayv (L, -1, lua_tointeger (L, lua_upvalueindex(1)), i, size);
    }		 
    
    return 1;
}

static int cast (lua_State *L)
{
    int i, j;

    for (i = 0 ; lua_type (L, i + 1) == LUA_TNUMBER ; i += 1);

    if (i < 1) {
	lua_pushstring (L, "Array dimensions undefined.");
	lua_error (L);
    }

    array_checkarray (L, i + 1);

    {
	int size[i];

	for (j = 0 ; j < i ; j += 1) {
	    size[j] = lua_tonumber (L, j + 1);
	}
	
	array_castv (L, i + 1, i, size);
    }		 
    
    return 1;
}

static int slice (lua_State *L)
{
    array_Array *array;
    int j;

    array = array_checkarray (L, 1);

    {
	int slices[array->rank * 2];

	for (j = 0 ; j < array->rank * 2 ; j += 1) {
	    slices[j] = luaL_checknumber (L, j + 2);
	}
	
	array_slicev (L, 1, slices);
    }		 
    
    return 1;
}

static int copy (lua_State *L)
{
    array_checkarray (L, 1);

    array_copy (L, 1);
    return 1;
}

static int set (lua_State *L)
{
    double c;
    
    array_checkarray (L, 1);
    c = luaL_checknumber (L, 2);

    array_set (L, 1, c);
    lua_pop (L, 1);
    
    return 1;
}

#define OP(FUNC, OPERATOR, TYPE)			\
    static void FUNC (TYPE *A, TYPE *B, TYPE *C, int n)	\
    {							\
	int i;						\
							\
	for (i = 0 ; i < n ; i += 1) {			\
	    C[i] = A[i] OPERATOR B[i];			\
	}						\
    }

#define DEFINE_OPERATION(OPERATION, OPERATOR)				\
    OP(OPERATION##_doubles, OPERATOR, double)				\
    OP(OPERATION##_floats, OPERATOR, float)				\
    OP(OPERATION##_ulongs, OPERATOR, unsigned long)			\
    OP(OPERATION##_longs, OPERATOR, signed long)			\
    OP(OPERATION##_uints, OPERATOR, unsigned int)			\
    OP(OPERATION##_ints, OPERATOR, signed int)				\
    OP(OPERATION##_ushorts, OPERATOR, unsigned short)			\
    OP(OPERATION##_shorts, OPERATOR, signed short)			\
    OP(OPERATION##_uchars, OPERATOR, unsigned char)			\
    OP(OPERATION##_chars, OPERATOR, signed char)			\
									\
    static int OPERATION (lua_State *L)					\
    {									\
	array_Array *A, *B, *C;						\
	int j, l;							\
									\
	A = array_checkarray (L, 1);					\
	B = array_checkarray (L, 2);					\
									\
	if (A->rank != B->rank) {					\
	    lua_pushstring (L, "Array dimensions don't match.");	\
	    lua_error (L);						\
	}								\
									\
	if (A->type != B->type) {					\
	    lua_pushstring (L, "Array types don't match.");		\
	    lua_error (L);						\
	}								\
									\
	for (j = 0 ,l = 1; j < A->rank ; j += 1) {			\
	    if (A->size[j] != B->size[j]) {				\
		lua_pushstring (L, "Array sizes don't match.");		\
		lua_error (L);						\
	    }								\
									\
	    l *= A->size[j];						\
	}								\
									\
	array_createarrayv (L, A->type, NULL, A->rank, A->size);	\
	C = (array_Array *)lua_touserdata (L, -1);			\
									\
	switch (A->type) {						\
	case ARRAY_TDOUBLE:						\
	    OPERATION##_doubles (A->values.doubles, B->values.doubles,	\
				 C->values.doubles, l);			\
	    break;							\
	case ARRAY_TFLOAT:						\
	    OPERATION##_floats (A->values.floats, B->values.floats,	\
				C->values.floats, l);			\
	    break;							\
	case ARRAY_TULONG:						\
	    OPERATION##_ulongs (A->values.ulongs, B->values.ulongs,	\
				C->values.ulongs, l);			\
	    break;							\
	case ARRAY_TLONG:						\
	    OPERATION##_longs (A->values.longs, B->values.longs,	\
			       C->values.longs, l);			\
	    break;							\
	case ARRAY_TUINT:						\
	    OPERATION##_uints (A->values.uints, B->values.uints,	\
			       C->values.uints, l);			\
	    break;							\
	case ARRAY_TINT:						\
	    OPERATION##_ints (A->values.ints, B->values.ints,		\
			      C->values.ints, l);			\
	    break;							\
	case ARRAY_TUSHORT:						\
	    OPERATION##_ushorts (A->values.ushorts, B->values.ushorts,	\
				 C->values.ushorts, l);			\
	    break;							\
	case ARRAY_TSHORT:						\
	    OPERATION##_shorts (A->values.shorts, B->values.shorts,	\
				C->values.shorts, l);			\
	    break;							\
	case ARRAY_TUCHAR:						\
	    OPERATION##_uchars (A->values.uchars, B->values.uchars,	\
				C->values.uchars, l);			\
	    break;							\
	case ARRAY_TCHAR:						\
	    OPERATION##_chars (A->values.chars, B->values.chars,	\
			       C->values.chars, l);			\
	    break;							\
	}								\
    									\
	return 1;							\
    }

DEFINE_OPERATION(add, +)
DEFINE_OPERATION(multiply, *)
DEFINE_OPERATION(subtract, -)
DEFINE_OPERATION(divide, /)

#define SCALE(FUNC, TYPE)				\
    static void FUNC (TYPE *A, TYPE c, TYPE *B, int n)	\
    {							\
	int i;						\
							\
	for (i = 0 ; i < n ; i += 1) {			\
	    B[i] = c * A[i];				\
	}						\
    }

SCALE(scale_doubles, double)
SCALE(scale_floats, float)
SCALE(scale_ulongs, unsigned long)
SCALE(scale_longs, signed long)
SCALE(scale_uints, unsigned int)
SCALE(scale_ints, signed int)
SCALE(scale_ushorts, unsigned short)
SCALE(scale_shorts, signed short)
SCALE(scale_uchars, unsigned char)
SCALE(scale_chars, signed char)

static int scale (lua_State *L)
{
    array_Array *A, *B;
    double c;
    int j, l;

    A = array_checkarray (L, 1);
    c = luaL_checknumber (L, 2);

    for (j = 0 ,l = 1; j < A->rank ; l *= A->size[j], j += 1);

    array_createarrayv (L, A->type, NULL, A->rank, A->size);
    B = (array_Array *)lua_touserdata (L, -1);

    switch (A->type) {
    case ARRAY_TDOUBLE:
	scale_doubles (A->values.doubles, c, B->values.doubles, l);
	break;
    case ARRAY_TFLOAT:
	scale_floats (A->values.floats, c, B->values.floats, l);
	break;
    case ARRAY_TULONG:
	scale_ulongs (A->values.ulongs, c, B->values.ulongs, l);
	break;
    case ARRAY_TLONG:
	scale_longs (A->values.longs, c, B->values.longs, l);
	break;
    case ARRAY_TUINT:
	scale_uints (A->values.uints, c, B->values.uints, l);
	break;
    case ARRAY_TINT:
	scale_ints (A->values.ints, c, B->values.ints, l);
	break;
    case ARRAY_TUSHORT:
	scale_ushorts (A->values.ushorts, c, B->values.ushorts, l);
	break;
    case ARRAY_TSHORT:
	scale_shorts (A->values.shorts, c, B->values.shorts, l);
	break;
    case ARRAY_TUCHAR:
	scale_uchars (A->values.uchars, c, B->values.uchars, l);
	break;
    case ARRAY_TCHAR:
	scale_chars (A->values.chars, c, B->values.chars, l);
	break;
    }

    return 1;
}

#define RAISE(FUNC, TYPE)				\
    static void FUNC (TYPE *A, TYPE c, TYPE *B, int n)	\
    {							\
	int i;						\
							\
	for (i = 0 ; i < n ; i += 1) {			\
	    B[i] = pow(A[i], c);			\
	}						\
    }

RAISE(raise_doubles, double)
RAISE(raise_floats, float)
RAISE(raise_ulongs, unsigned long)
RAISE(raise_longs, signed long)
RAISE(raise_uints, unsigned int)
RAISE(raise_ints, signed int)
RAISE(raise_ushorts, unsigned short)
RAISE(raise_shorts, signed short)
RAISE(raise_uchars, unsigned char)
RAISE(raise_chars, signed char)

static int raise (lua_State *L)
{
    array_Array *A, *B;
    double c;
    int j, l;

    A = array_checkarray (L, 1);
    c = luaL_checknumber (L, 2);

    for (j = 0, l = 1; j < A->rank ; l *= A->size[j], j += 1);

    array_createarrayv (L, A->type, NULL, A->rank, A->size);
    B = (array_Array *)lua_touserdata (L, -1);

    switch (A->type) {
    case ARRAY_TDOUBLE:
	raise_doubles (A->values.doubles, c, B->values.doubles, l);
	break;
    case ARRAY_TFLOAT:
	raise_floats (A->values.floats, c, B->values.floats, l);
	break;
    case ARRAY_TULONG:
	raise_ulongs (A->values.ulongs, c, B->values.ulongs, l);
	break;
    case ARRAY_TLONG:
	raise_longs (A->values.longs, c, B->values.longs, l);
	break;
    case ARRAY_TUINT:
	raise_uints (A->values.uints, c, B->values.uints, l);
	break;
    case ARRAY_TINT:
	raise_ints (A->values.ints, c, B->values.ints, l);
	break;
    case ARRAY_TUSHORT:
	raise_ushorts (A->values.ushorts, c, B->values.ushorts, l);
	break;
    case ARRAY_TSHORT:
	raise_shorts (A->values.shorts, c, B->values.shorts, l);
	break;
    case ARRAY_TUCHAR:
	raise_uchars (A->values.uchars, c, B->values.uchars, l);
	break;
    case ARRAY_TCHAR:
	raise_chars (A->values.chars, c, B->values.chars, l);
	break;
    }

    return 1;
}

int luaopen_array_core (lua_State *L)
{
    const luaL_Reg api[] = {
	{"add", add},
	{"multiply", multiply},
	{"subtract", subtract},
	{"divide", divide},
	{"scale", scale},
	{"raise", raise},
    
	{"copy", copy},
	{"set", set},
	{"cast", cast},
	{"slice", slice},
	{NULL, NULL}
    };


#if LUA_VERSION_NUM == 501
    luaL_register (L, "array", api);
#else
    luaL_newlib (L, api);
#endif

    /* Create an array object out of a Lua table or string. */
    
    lua_pushinteger (L, ARRAY_TDOUBLE);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "doubles");
    
    lua_pushinteger (L, ARRAY_TFLOAT);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "floats");
    
    lua_pushinteger (L, ARRAY_TULONG);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "ulongs");
    
    lua_pushinteger (L, ARRAY_TLONG);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "longs");
    
    lua_pushinteger (L, ARRAY_TUINT);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "uints");
    
    lua_pushinteger (L, ARRAY_TINT);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "ints");
    
    lua_pushinteger (L, ARRAY_TUSHORT);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "ushorts");
    
    lua_pushinteger (L, ARRAY_TSHORT);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "shorts");
    
    lua_pushinteger (L, ARRAY_TUCHAR);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "uchars");
    
    lua_pushinteger (L, ARRAY_TCHAR);
    lua_pushcclosure (L, create, 1);
    lua_setfield (L, -2, "chars");    

    /* Create arrays from other arrays. */
    
    return 1;
}
