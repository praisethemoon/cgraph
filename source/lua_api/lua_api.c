#include "lua.h"
#include "lauxlib.h"

#include "array.h"

#include "../cg_factory.h"
#include "../cg_types.h"
#include "../ccgraph.h"
#include "../cg_operation.h"
#include "../cg_constants.h"
#include "../cg_errors.h"
#include "../cg_variables.h"

/*
 * TODO: Check lua input stack parameters
 */

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

static int lua_createVariable(lua_State* L){
	const char* name = lua_tostring(L, 1);
	printf("Creating var with name %s\n", name);
	CGNode* node = makeVarNode(name);
	
	lua_pushlightuserdata(L, node);
	return 1;
}

static int lua_createDoubleConstant(lua_State* L){
	double value = lua_tonumber(L, 1);
	printf("Creating constant with value %f\n", value);
	CGNode* node = makeDoubleConstantNode(value);
	
	lua_pushlightuserdata(L, node);
}


static int lua_createVectorConstant(lua_State* L){
	uint64_t len = lua_tointeger(L, 1);
	array_Array* values = array_checkarray(L, 2);
	
	
	printf("Creating vector array with length %d\n", len);
	uint64_t i = 0;
	for(; i < len; i++){
		printf("\t%f\n", values->values.doubles[i]);
	}
	
	CGNode* node = makeVectorConstantNode(len, values);
	
	lua_pushlightuserdata(L, node);
}

static int lua_createMatrixConstant(lua_State* L){
	uint64_t rows = lua_tointeger(L, 1);
	uint64_t cols = lua_tointeger(L, 2);
	array_Array* values = array_checkarray(L, 3);
	
	
	printf("Creating matrix array with length %dx%d\n", rows, cols);
	uint64_t i = 0;
	uint64_t j = 0;
	
	for(;i<rows;i++){
		for(j = 0;j<cols;j++){
			printf("\t%lf", values->values.doubles[i*cols +j]);
		}
		printf("\n");
	}
	
	CGNode* node = makeMatrixConstantNode(rows, cols, values);
	
	lua_pushlightuserdata(L, node);
}

int luaopen_libcgraph(lua_State *L)
{
	struct luaL_Reg driver[] =
	{
		{"var", lua_createVariable},
		{"double", lua_createDoubleConstant},
		{"vector", lua_createVectorConstant},
		{"matrix", lua_createMatrixConstant},
		{NULL, NULL}
	};

	luaL_openlib(L, "cgraph", driver, 0);
	
	lua_pushinteger (L, ARRAY_TDOUBLE);
	lua_pushcclosure (L, create, 1);
	lua_setfield (L, -2, "doubles");
	return 1;
}
