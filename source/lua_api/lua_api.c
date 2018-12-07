#include "lua.h"
#include "lauxlib.h"

#include "array.h"

#include "cg_factory.h"
#include "cg_types.h"
#include "cgraph.h"
#include "cg_operation.h"
#include "cg_constants.h"
#include "cg_errors.h"
#include "cg_variables.h"
#include "cg_diff.h"
#include "cg_enums.h"

#define CGNODE "CGNode"
#define CGRAPH "CGraph"

/* source: https://www.lua.org/pil/24.2.3.html 
 * used when debugging lua with gdb
 */
static void stackDump (lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {
			case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

			default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;
		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

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


static CGNode* checkNode(lua_State* L, int index){
	CGNode* node = NULL;
	//luaL_checktype(L, index, LUA_TUSERDATA);
	node = (CGNode*)luaL_checkudata(L, index, CGNODE);
	
	return node;
}

static void pushNode(lua_State* L, CGNode* node){
	//CGNode* lnode = (CGNode *)lua_newuserdata(L, sizeof(CGNode));
	//*lnode = *node;
	lua_pushlightuserdata(L, node);
	luaL_getmetatable(L, CGNODE);
	lua_setmetatable(L, -2);
}

static CGraph* checkGraph(lua_State* L, int index){
	CGraph* graph = NULL;
	//luaL_checktype(L, index, LUA_TUSERDATA);
	graph = (CGraph*)luaL_checkudata(L, index, CGRAPH);
	
	return graph;
}

static void pushGraph(lua_State* L, CGraph* graph){
	CGraph* lgraph= (CGraph *)lua_newuserdata(L, sizeof(CGraph));
	*lgraph = *graph;
	
	//lua_pushlightuserdata(L, graph);
	luaL_getmetatable(L, CGRAPH);
	lua_setmetatable(L, -2);
}

static int lua_getBinaryOperationTypeString(lua_State* L){
	uint64_t i = lua_tointeger(L, 1);
	
	lua_pushstring(L, getBinaryOperationTypeString(i));
	return 1;
}


static int lua_getUnaryOperationTypeString(lua_State* L){
	uint64_t i = lua_tointeger(L, 1);
	
	lua_pushstring(L, getUnaryOperationTypeString(i));
	return 1;
}

static int lua_getNodeTypeString(lua_State* L){
	uint64_t i = lua_tointeger(L, 1);
	
	lua_pushstring(L, getNodeTypeString(i));
	return 1;
}

static int lua_getVariableTypeString(lua_State* L){
	uint64_t i = lua_tointeger(L, 1);
	
	lua_pushstring(L, getVariableTypeString(i));
	return 1;
}

static int lua_getErrorTypeString(lua_State* L){
	uint64_t i = lua_tointeger(L, 1);
	
	lua_pushstring(L, getErrorTypeString(i));
	return 1;
}

/* NOT USED 
 * TODO: Remove this 
 */
static int lua_getBinaryOperations(lua_State* L){
	lua_newtable(L);
	uint8_t i = 0;
	for(;i <= MAX_BINARY_OPERATION;i++){
		lua_pushstring(L, getBinaryOperationTypeString(i));
		lua_pushinteger(L, i);
		lua_settable(L, -3);
	}
	
	return 1;
}

static int lua_createVariable(lua_State* L){
	const char* name = lua_tostring(L, 1);
	//printf("Creating var with name %s\n", name);
	CGNode* node = makeVarNode(name);
	
	pushNode(L, node);
	return 1;
}

static int lua_createDoubleConstant(lua_State* L){
	double value = lua_tonumber(L, 1);
	//printf("Creating constant with value %f\n", value);
	CGNode* node = makeDoubleConstantNode(value);
	
	pushNode(L, node);
	return 1;
}


static int lua_createVectorConstant(lua_State* L){
	uint64_t len = lua_tointeger(L, 1);
	array_Array* values = array_checkarray(L, 2);
	
	/*
	printf("Creating vector array with length %d\n", len);
	uint64_t i = 0;
	for(; i < len; i++){
		printf("\t%f\n", values->values.doubles[i]);
	}
	*/
	double* data = calloc(len, sizeof(double));
	memcpy(data, values->values.doubles, len*sizeof(double));
	
	CGNode* node = makeVectorConstantNode(len, data);
	
	pushNode(L, node);
	return 1;
}

static int lua_createGraphNode(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	CGNode* node = makeGraphNode(graph);
	
	pushNode(L, node);
	return 1;
}

static int lua_createMatrixConstant(lua_State* L){
	uint64_t rows = lua_tointeger(L, 1);
	uint64_t cols = lua_tointeger(L, 2);
	array_Array* values = array_checkarray(L, 3);
	
	/*
	printf("Creating matrix array with length %dx%d\n", rows, cols);
	uint64_t i = 0;
	uint64_t j = 0;
	
	for(;i<rows;i++){
		for(j = 0;j<cols;j++){
			printf("\t%lf", values->values.doubles[i*cols +j]);
		}
		printf("\n");
	}
	*/
	
	double* data = calloc( rows*cols, sizeof(double));
	memcpy(data, values->values.doubles, rows*cols*sizeof(double));
	CGNode* node = makeMatrixConstantNode(rows, cols, data);
	
	pushNode(L, node);
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
	CGNode* lhs = checkNode(L, 2);
	CGNode* rhs = checkNode(L, 3);
	
	//printf("creating %s %zu %zu\n", getBinaryOperationTypeString(type), lhs, rhs);
	
	CGNode* node = makeBinaryOpNode(ops[type], lhs, rhs);
	//printf("res -> %zu\n", node);
	
	pushNode(L, node);
	return 1;
}


static int lua_createUnaryOperation(lua_State* L){
	const CGUnaryOperationType ops[] = {
		CGUOT_MINUS,
		CGUOT_INV,
		CGUOT_TRANSPOSE,
		CGUOT_EXP,
		CGUOT_LOG,
		CGUOT_SIN,
		CGUOT_COS,
		CGUOT_TAN,
		CGUOT_TANH,
		CGUOT_SUM,
	};
	
	uint8_t type = lua_tointeger(L, 1);
	CGNode* uhs = checkNode(L, 2);
	CGNode* node = makeUnaryOpNode(ops[type], uhs);
	
	pushNode(L, node);
	return 1;
}

static int lua_createGraph(lua_State* L){
	const char* name = lua_tostring(L, 1);
	CGNode* node = checkNode(L, 2);
	//printf("Creating graph with name %s and node %zu\n", name, node);
	CGraph* graph = makeGraph(name);
	graph->root = node;
	
	pushGraph(L, graph);
	return 1;
}

static int lua_setGraphVar(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	const char* name = lua_tostring(L, 2);
	CGNode* node = checkNode(L, 3);
	
	graphSetVar(graph, name, node);
	
	lua_pushnil(L);
	return 1;
}

static int lua_getGraphVar(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	const char* name = lua_tostring(L, 2);
	
	CGNode* node = graphGetVar(graph, name);
	
	if(node == NULL)
	{
		lua_pushnil(L);
	}
	else {
		pushNode(L, node);
	}
	
	return 1;
}

static int lua_computeGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	CGResultNode* res = computeGraph(graph);
	
	if(res->error != NULL)
	{
		lua_newtable(L);
		lua_pushstring(L, "error");
		lua_pushinteger(L, (int)res->error->errorType);
		lua_settable(L, -3);
		lua_pushstring(L, "message");
		lua_pushstring(L, res->error->message);
		lua_settable(L, -3);
		lua_pushstring(L, "node");
		pushNode(L, res->error->faultyNode);
		lua_settable(L, -3);
		
		return 1;
	}
	
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushinteger(L, res->type);
	lua_settable(L, -3);
	
	switch(res->type){
		case CGVT_DOUBLE:{
			CGDouble* value = (CGDouble*)res->value;
			
			lua_pushstring(L, "value");
			lua_pushnumber(L, value->value);
			lua_settable(L, -3);
			break;
		}
		
			
		case CGVT_VECTOR:{
			CGVector* value = (CGVector*)res->value;
			
			/*
			uint64_t i = 0;
			printf("Vector len: %d\n", value->len);
			for(;i<value->len;i++){
				printf("\t%d\t%lf\n", i, value->data[i]);
			}
			*/
			
			lua_pushstring(L, "len");
			lua_pushnumber(L, value->len);
			lua_settable(L, -3);
			int size[]= {0};
			size[0] = value->len;
			
			lua_pushstring(L, "value");
			array_createarrayv (L, ARRAY_TDOUBLE, value->data, 1, size);
			lua_settable(L, -3);
			break;
		}
			
		case CGVT_MATRIX:{
			CGMatrix* value = (CGMatrix*)res->value;
			/*
			printf("Matrix len: %dx%d\n", value->rows, value->cols);
			
			uint64_t i = 0;
			uint64_t j = 0;
			for(;i<value->rows;i++){
				for(j = 0;j<value->cols;j++){
					printf("\t%lf", value->data[i*value->cols +j]);
				}
				printf("\n");
			}
			*/
			lua_pushstring(L, "rows");
			lua_pushnumber(L, value->rows);
			lua_settable(L, -3);
			
			lua_pushstring(L, "cols");
			lua_pushnumber(L, value->cols);
			lua_settable(L, -3);
			
			int size[]= {0};
			size[0] = value->rows*value->cols;
			
			lua_pushstring(L, "value");
			array_createarrayv (L, ARRAY_TDOUBLE, value->data, 1, size);
			lua_settable(L, -3);
			break;
		}
	}
	
	return 1;
}


void nodeToLuaTable(CGNode* node, lua_State* L, CGraph* graph){
	lua_newtable(L);
	
	switch(node->type){
		case CGNT_BINARY_OPERATION: {
			lua_pushstring(L, "type");
			lua_pushstring(L, "bop");
			lua_settable(L, -3);
			
			lua_pushstring(L, "opType");
			lua_pushinteger(L, node->bop->type);
			lua_settable(L, -3);
			
			lua_pushstring(L, "lhs");
			nodeToLuaTable(node->bop->lhs, L, graph);
			lua_settable(L, -3);
			
			lua_pushstring(L, "rhs");
			nodeToLuaTable(node->bop->rhs, L, graph);
			lua_settable(L, -3);
			
			lua_pushstring(L, "node");
			pushNode(L, node);
			printf("Checking stack: %d\n", lua_checkstack(L, 20));
			lua_settable(L, -3);
			
			return;
		}
		
		case CGNT_UNARY_OPERATION: {
			lua_pushstring(L, "type");
			lua_pushstring(L, "uop");
			lua_settable(L, -3);
			
			lua_pushstring(L, "opType");
			lua_pushinteger(L, node->uop->type);
			lua_settable(L, -3);
			
			lua_pushstring(L, "uhs");
			nodeToLuaTable(node->uop->uhs, L, graph);
			lua_settable(L, -3);
			
			lua_pushstring(L, "node");
			pushNode(L, node);
			lua_settable(L, -3);
			
			return;
		}
		
		case CGNT_VARIABLE: {
			lua_pushstring(L, "type");
			lua_pushstring(L, "var");
			lua_settable(L, -3);
			
			lua_pushstring(L, "name");
			lua_pushstring(L, node->var->name);
			lua_settable(L, -3);
			
			lua_pushstring(L, "node");
			pushNode(L, node);
			lua_settable(L, -3);
			
			return;
		}
		
		
		case CGNT_CONSTANT: {
			lua_pushstring(L, "type");
			lua_pushstring(L, "value");
			lua_settable(L, -3);
			
			lua_pushstring(L, "tensorType");
			lua_pushinteger(L, node->constant->type);
			lua_settable(L, -3);
			
			lua_pushstring(L, "node");
			pushNode(L, node);
			lua_settable(L, -3);
			
			switch(node->constant->type){
				case CGVT_DOUBLE:{
					CGDouble* value = (CGDouble*)node->constant->value;
					
					lua_pushstring(L, "value");
					lua_pushnumber(L, value->value);
					lua_settable(L, -3);
					break;
				}
				
					
				case CGVT_VECTOR:{
					CGVector* value = (CGVector*)node->constant->value;
					
					/*
					uint64_t i = 0;
					printf("Vector len: %d\n", value->len);
					for(;i<value->len;i++){
						printf("\t%d\t%lf\n", i, value->data[i]);
					}
					*/
					
					lua_pushstring(L, "len");
					lua_pushnumber(L, value->len);
					lua_settable(L, -3);
					int size[]= {0};
					size[0] = value->len;
					
					lua_pushstring(L, "value");
					array_createarrayv (L, ARRAY_TDOUBLE, value->data, 1, size);
					lua_settable(L, -3);
					break;
				}
					
				case CGVT_MATRIX:{
					CGMatrix* value = (CGMatrix*)node->constant->value;
					/*
					printf("Matrix len: %dx%d\n", value->rows, value->cols);
					
					uint64_t i = 0;
					uint64_t j = 0;
					for(;i<value->rows;i++){
						for(j = 0;j<value->cols;j++){
							printf("\t%lf", value->data[i*value->cols +j]);
						}
						printf("\n");
					}
					*/
					lua_pushstring(L, "rows");
					lua_pushnumber(L, value->rows);
					lua_settable(L, -3);
					
					lua_pushstring(L, "cols");
					lua_pushnumber(L, value->cols);
					lua_settable(L, -3);
					
					int size[]= {0};
					size[0] = value->rows*value->cols;
					
					lua_pushstring(L, "value");
					array_createarrayv (L, ARRAY_TDOUBLE, value->data, 1, size);
					lua_settable(L, -3);
					break;
				}
			}
			return;
		}
		
		case CGNT_GRAPH:
		{
			// TODO: Generate LUA Graph that must  be reinterpreted by lua API
		}
	}
}

static int lua_diffGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	char* newName = lua_tostring(L, 2);
	CGNode* wrtNode = checkNode(L, 3);
	
	CGraph* newGraph = differentiateGraphWRTNode(graph, newName, wrtNode);
	
	lua_newtable(L);
	
	lua_pushstring(L, "graph");
	pushGraph(L, newGraph);
	lua_settable(L, -3);
	lua_pushstring(L, "root");
	nodeToLuaTable(newGraph->root, L, newGraph);
	lua_settable(L, -3);
	
	return 1;
}

static int lua_freeGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	
	if(graph->root != NULL){
		freeGraph(graph);
		graph->root = NULL;
	}
	
	lua_pushnil(L);
	return 1;
}

static int lua_optimizeGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	
	if(graph->root != NULL){
		optimizeGraph(graph);
		lua_newtable(L);
		lua_pushstring(L, "graph");
		pushGraph(L, graph);
		lua_settable(L, -3);
		lua_pushstring(L, "root");
		nodeToLuaTable(graph->root, L, graph);
		lua_settable(L, -3);
	}
	else
		lua_pushnil(L);
	
	return 1;
}

int luaopen_libcgraph(lua_State *L)
{
	struct luaL_Reg driver[] =
	{
		{"bopToString", lua_getBinaryOperationTypeString},
		{"uopToString", lua_getUnaryOperationTypeString},
		{"nodeTypeToString", lua_getNodeTypeString},
		{"varTypeToString", lua_getVariableTypeString},
		{"errorTypeToString", lua_getErrorTypeString},
		{"var", lua_createVariable},
		{"double", lua_createDoubleConstant},
		{"vector", lua_createVectorConstant},
		{"matrix", lua_createMatrixConstant},
		{"bop", lua_createBinaryOperation},
		{"uop", lua_createUnaryOperation},
		{"graphNode", lua_createGraphNode},
		{"graph", lua_createGraph},
		{"setVar", lua_setGraphVar},
		{"getVar", lua_getGraphVar},
		{"compute", lua_computeGraph},
		{"diff", lua_diffGraph},
		{"optimizeGraph", lua_optimizeGraph},
		{"freeGraph", lua_freeGraph},
		{NULL, NULL}
	};

	luaL_openlib(L, "cgraph", driver, 0);
	
	lua_pushinteger (L, ARRAY_TDOUBLE);
	lua_pushcclosure (L, create, 1);
	lua_setfield (L, -2, "doubles");
	
	static const luaL_Reg CGRAPH_meta[] = {
		{"__gc", lua_freeGraph},
		{0, 0}
	};
	
	/* TODO: set gc to metatable */
	luaL_newmetatable(L, CGNODE);  
	luaL_newmetatable(L, CGRAPH);
	return 1;
}
