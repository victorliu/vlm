#include "lua_compat.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int lcl_len(lua_State *L, int idx){
	int len = 0;
#if LUA_VERSION_NUM < 502
	len = lua_objlen(L, idx);
#else
	lua_len(L, idx);
	len = lua_tointeger(L, -1);
	lua_pop(L, 1);
#endif
	return len;
}

