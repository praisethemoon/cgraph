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
	
	return 1;
}


static int lua_createVectorConstant(lua_State* L){
	uint64_t len = lua_tointeger(L, 1);
	array_Array* values = array_checkarray(L, 2);
	
	
	printf("Creating vector array with length %d\n", len);
	uint64_t i = 0;
	for(; i < len; i++){
		printf("\t%f\n", values->values.doubles[i]);
	}
	
	double* data = dmt_calloc(len, sizeof(double));
	memcpy(data, values->values.doubles, len*sizeof(double));
	
	CGNode* node = makeVectorConstantNode(len, data);
	
	lua_pushlightuserdata(L, node);
	
	return 1;
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
	
	double* data = dmt_calloc( rows*cols, sizeof(double));
	memcpy(data, values->values.doubles, rows*cols*sizeof(double));
	CGNode* node = makeMatrixConstantNode(rows, cols, data);
	
	lua_pushlightuserdata(L, node);
	
	return 1;
}

static int lua_createBinaryOperation(lua_State* L){
	const CGBinaryOperationType ops[] = {
		CGBOT_ADD,
		CGBOT_SUB,
		CGBOT_MULT,
		CGBOT_DIV,
		CGBOT_POW,
		CGBOT_DOT,
		CGBOT_TMULT
	};
	
	uint8_t type = lua_tointeger(L, 1);
	CGNode* lhs = lua_touserdata(L, 2);
	CGNode* rhs = lua_touserdata(L, 3);
	
	printf("creating %s %zu %zu\n", getBinaryOperationTypeString(type), lhs, rhs);
	
	CGNode* node = makeBinaryOpNode(ops[type], lhs, rhs);
	printf("res -> %zu\n", node);
	
	lua_pushlightuserdata(L, node);
	
	return 1;
}


static int lua_createUnaryOperation(lua_State* L){
	const CGUnaryOperationType ops[] = {
		CGUOT_MINUS,
		CGUOT_INV,
		CGUOT_TRANSPOSE,
		CGUOT_EXP,
		CGUOT_LOG,
	};
	
	uint8_t type = lua_tointeger(L, 1);
	CGNode* uhs = lua_touserdata(L, 2);
	CGNode* node = makeUnaryOpNode(ops[type], uhs);
	
	lua_pushlightuserdata(L, node);
	
	return 1;
}

static int lua_createGraph(lua_State* L){
	const char* name = lua_tostring(L, 1);
	CGNode* node = lua_touserdata(L, 2);
	printf("Creating graph with name %s and node %zu\n", name, node);
	CGraph* graph = makeGraph(name);
	graph->root = node;
	
	lua_pushlightuserdata(L, graph);
	return 1;
}

static int lua_setGraphVar(lua_State* L){
	CGraph* graph = lua_touserdata(L, 1);
	const char* name = lua_tostring(L, 2);
	CGNode* node = lua_touserdata(L, 3);
	
	graphSetVar(graph, name, node);
	
	lua_pushnil(L);
	
	return 1;
}

static int lua_computeGraph(lua_State* L){
	CGraph* graph = lua_touserdata(L, 1);
	CGResultNode* res = computeGraph(graph);
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushinteger(L, res->type);
	lua_settable(L, -3);
	
	switch(res->type){
		case CGVT_DOUBLE:{
			CGDouble* value = (CGDouble*)res->value;
			lua_pushnumber(L, value->value);
			break;
		}
		
			
		case CGVT_VECTOR:{
			CGVector* value = (CGVector*)res->value;
			
			
			uint64_t i = 0;
			printf("Vector len: %d\n", value->len);
			for(;i<value->len;i++){
				printf("\t%d\t%lf\n", i, value->data[i]);
			}
			
			lua_pushstring(L, "len");
			lua_pushnumber(L, value->len);
			lua_settable(L, -3);
			int size[]= {0};
			size[0] = value->len;
			array_createarrayv(L, ARRAY_TDOUBLE, value->data, 1, size);
			break;
		}
			
		case CGVT_MATRIX:{
			CGMatrix* value = (CGMatrix*)res->value;
			printf("Matrix len: %dx%d\n", value->rows, value->cols);
			
			uint64_t i = 0;
			uint64_t j = 0;
			for(;i<value->rows;i++){
				for(j = 0;j<value->cols;j++){
					printf("\t%lf", value->data[i*value->cols +j]);
				}
				printf("\n");
			}
			lua_pushstring(L, "rows");
			lua_pushnumber(L, value->rows);
			lua_settable(L, -3);
			
			lua_pushstring(L, "cols");
			lua_pushnumber(L, value->cols);
			lua_settable(L, -3);
			
			int size[]= {0};
			size[0] = value->rows*value->cols;
			array_createarrayv (L, ARRAY_TDOUBLE, value->data, 1, size);
			break;
		}
	}
	
	return 2;
}


int luaopen_libcgraph(lua_State *L)
{
	struct luaL_Reg driver[] =
	{
		{"var", lua_createVariable},
		{"double", lua_createDoubleConstant},
		{"vector", lua_createVectorConstant},
		{"matrix", lua_createMatrixConstant},
		{"bop", lua_createBinaryOperation},
		{"uop", lua_createUnaryOperation},
		{"graph", lua_createGraph},
		{"setVar", lua_setGraphVar},
		{"compute", lua_computeGraph},
		{NULL, NULL}
	};

	luaL_openlib(L, "cgraph", driver, 0);
	
	lua_pushinteger (L, ARRAY_TDOUBLE);
	lua_pushcclosure (L, create, 1);
	lua_setfield (L, -2, "doubles");
	return 1;
}
