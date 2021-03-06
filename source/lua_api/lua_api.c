#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
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
#include "cg_plot.h"

#include "progressbar.h"



#if cg_float == float
	#define LUA_C_TYPE ARRAY_TFLOAT
	#define lua_float floats
#else
	#define LUA_C_TYPE ARRAY_TDOUBLE
	#define lua_float doubles
#endif


#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
/* Compatibility for Lua 5.1.
 *
 * luaL_setfuncs() is used to create a module table where the functions have
 * json_config_t as their first upvalue. Code borrowed from Lua 5.2 source. */
static void luaL_setfuncs (lua_State *l, const luaL_Reg *reg, int nup)
{
    int i;

    luaL_checkstack(l, nup, "too many upvalues");
    for (; reg->name != NULL; reg++) {  /* fill the table with given functions */
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(l, -nup);
        lua_pushcclosure(l, reg->func, nup);  /* closure with those upvalues */
        lua_setfield(l, -(nup + 2), reg->name);
    }
    lua_pop(l, nup);  /* remove upvalues */
}
#endif

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
	CGNode* lnode = (CGNode *)lua_newuserdata(L, sizeof(CGNode));
	*lnode = *node;
	free(node);
	//lua_pushlightuserdata(L, node);
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
	free(graph);
	
	//lua_pushlightuserdata(L, graph);
	luaL_getmetatable(L, CGRAPH);
	lua_setmetatable(L, -2);
}

static int lua_init(lua_State* L){

    selectContext();

    lua_pushnil(L);
    return 1;
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
	cg_float value = lua_tonumber(L, 1);
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
	cg_float* data = calloc(len, sizeof(cg_float));
	memcpy(data, values->values.lua_float, len*sizeof(cg_float));
	
	CGNode* node = makeVectorConstantNode(len, data);
	
	pushNode(L, node);
	return 1;
}

// DO NOT USE YET
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
			printf("\t%lf", values->values.lua_float[i*cols +j]);
		}
		printf("\n");
	}
	*/
	
	cg_float* data = calloc( rows*cols, sizeof(cg_float));
	memcpy(data, values->values.lua_float, rows*cols*sizeof(cg_float));
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
		CGUOT_RELU,
	};
	
	uint8_t type = lua_tointeger(L, 1);
	CGNode* uhs = checkNode(L, 2);
	CGNode* node = makeUnaryOpNode(ops[type], uhs);
	
	pushNode(L, node);
	return 1;
}

static int lua_createAxisBoundOp(lua_State* L){
	const CGAxisBoundOperationType ops[] = {
			CGABOT_SUM,
			CGABOT_MIN,
			CGABOT_MAX,
			CGABOT_MEAN,
			CGABOT_VARIANCE,
			CGABOT_SOFTMAX,
			CGABOT_ARGMIN,
			CGABOT_ARGMAX
	};
	// TODO: Assert
	CGNode* uhs = checkNode(L, 1);
	uint8_t type = lua_tointeger(L, 2);
	uint8_t axis = lua_tointeger(L, 3);
	CGNode* node = makeAxisBoundNode(ops[type], uhs, axis);

	pushNode(L, node);
	return 1;
}

static int lua_createSumOperation(lua_State* L){
	CGNode* uhs = checkNode(L, 1);
	uint8_t axis = lua_tointeger(L, 2);
	CGNode* node = makeAxisBoundNode(CGABOT_SUM, uhs, axis);
	
	pushNode(L, node);
	return 1;
}


static int lua_createCrossEntropyLoss(lua_State* L){
	CGNode* x = checkNode(L, 1);
	CGNode* y = checkNode(L, 2);
	uint64_t num_classes = lua_tointeger(L, 3);
	CGNode* node = makeCrossEntropyLossFunc(x, y, num_classes);
	
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

    graphSetVar_lua(graph, name, node);
	
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

static int lua_UnsetGraphVar(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	const char* name = lua_tostring(L, 2);
	
	graphUnsetVar(graph, name);
	
	return 1;
}

static int lua_freeNodeFromGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	CGNode* node = checkNode(L, 2);
	
	freeNode_lua(graph, node);
	//free(node);
	
	return 1;
}


void pushResultNode(lua_State*L, CGResultNode* res){

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
			array_createarrayv (L, LUA_C_TYPE, value->data, 1, size);
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
			array_createarrayv (L, LUA_C_TYPE, value->data, 1, size);
			lua_settable(L, -3);
			break;
		}
	}
}

static int lua_computeGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	CGResultNode* res = computeGraph(graph);
	pushResultNode(L, res);
	
	return 1;
}

static int lua_computeGraphNode(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	CGNode* node = checkNode(L, 2);
	CGResultNode* res = computeGraphNode(graph, node);
	pushResultNode(L, res);
	
	return 1;
}

//@Deprecated
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
					array_createarrayv (L, LUA_C_TYPE, value->data, 1, size);
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
					array_createarrayv (L, LUA_C_TYPE, value->data, 1, size);
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

static int lua_backPropGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	
	autoDifferenciateGraph(graph);

	return 1;
}

static int lua_getGraphVarDiff(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	const char* name = lua_tostring(L, 2);
	
	CGNode* node = graphGetVar(graph, name);
	
	if(node == NULL || node->diff == NULL)
	{
		lua_pushnil(L);
	}
	else {
		pushResultNode(L, constantNodeToResultNode(node->diff));
		//pushNode(L, node->diff);
	}
	
	return 1;
}


static int lua_freeNode(lua_State* L){
	CGNode* node = checkNode(L, 1);
	//printf("freeing solo node\n");
	
	freeNode(NULL, node);
	//free(node);
	
	lua_pushnil(L);
	return 1;
}

static int lua_freeGraph(lua_State* L){
	CGraph* graph = checkGraph(L, 1);
	printf("freeing graph %s\n", graph->name);
	if(graph->root != NULL){
		freeGraph_lua(graph);
	}
	
	lua_pushnil(L);
	return 1;
}

static int lua_startProgress(lua_State* L){
    struct progressbar *progress = progressbar_new(lua_tostring(L, 1),lua_tointeger(L, 2));
    lua_pushlightuserdata(L, progress);

    return 1;
}

static int lua_updateProgress(lua_State* L){
    struct progressbar *progress = lua_touserdata(L, 1);
    progressbar_inc(progress);

    return 1;
}

static int lua_stopProgress(lua_State* L){
    struct progressbar *progress = lua_touserdata(L, 1);
    progressbar_finish(progress);

    lua_pushnil(L);
    return 1;
}

static int lua_plotLines(lua_State* L){
	array_Array* xvalues = array_checkarray(L, 1);
	int size = xvalues->size[0];
	cg_float* xdata = calloc(xvalues->size[0], sizeof(cg_float));
	memcpy(xdata, xvalues->values.lua_float, size*sizeof(cg_float));

	array_Array* yvalues = array_checkarray(L, 2);
	cg_float* ydata = calloc(yvalues->size[0], sizeof(cg_float));
	memcpy(ydata, yvalues->values.lua_float, yvalues->size[0]*sizeof(cg_float));

	if(yvalues->size[0] != xvalues->size[0]){
		fprintf(stderr, "cannot plot x-y arrays with size %d, %d\n", xvalues->size[0], yvalues->size[0]);
		exit(-1);
	}

	const char* name = lua_tostring(L, 3);

	printf("plotting %d %f %f \n", size, xdata[0], ydata[0]);

	cg_plot(size, xdata, ydata, NULL, name);

	lua_pushnil(L);

	return 1;
}

int luaopen_libcgraph(lua_State *L)
{
	struct luaL_Reg driver[] =
	{
        {"init", lua_init},
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
		{"abop", lua_createAxisBoundOp},
		{"sum", lua_createSumOperation},
		{"graphNode", lua_createGraphNode},
		{"graph", lua_createGraph},
		{"setVar", lua_setGraphVar},
		{"getVar", lua_getGraphVar},
		{"unsetVar", lua_UnsetGraphVar},
		{"compute", lua_computeGraph},
		{"computeNode", lua_computeGraphNode},
		{"crossEntropy", lua_createCrossEntropyLoss},
		{"backProp", lua_backPropGraph},
		{"getVarDiff", lua_getGraphVarDiff},
		{"freeGraph", lua_freeGraph},
		{"freeGraphNode", lua_freeNodeFromGraph},
		{"freeNode", lua_freeNode},
        {"startProgress", lua_startProgress},
        {"updateProgress", lua_updateProgress},
        {"endProgress", lua_stopProgress},
		{"plotLines", lua_plotLines},
		{NULL, NULL}
	};

	luaL_openlib(L, "libcgraph", driver, 0);
	
	lua_pushinteger (L, LUA_C_TYPE);
	lua_pushcclosure (L, create, 1);
	lua_setfield (L, -2, "doubles");
	
	static const luaL_Reg CGRAPH_meta[] = {
		{"__gc", lua_freeGraph},
		{0, 0}
	};
	
	static const luaL_Reg CGNODE_meta[] = {
		{"__gc", lua_freeNode},
		{0, 0}
	};

	/* TODO: set gc to metatable */
	luaL_newmetatable(L, CGNODE);
	luaL_setfuncs(L, CGNODE_meta, 0);
	luaL_newmetatable(L, CGRAPH);
	luaL_setfuncs(L, CGRAPH_meta, 0);
	return 1;
}

#undef LUA_C_TYPE
