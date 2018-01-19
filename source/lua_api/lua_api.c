#include "lua.h"
#include "lauxlib.h"
#include "../cg_factory.h"
#include "../cg_types.h"
#include "../ccgraph.h"
#include "../cg_operation.h"
#include "../cg_constants.h"
#include "../cg_errors.h"
#include "../cg_variables.h"

static int lua_createVariable(lua_State* L){
	const char* 
	CGNode* var = makeVarNode();
	
	lua_pushlightuserdata(L, var);
	return 1;
}
