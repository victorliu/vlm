#include <stdlib.h>
#include <math.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "../lua_compat.h"
#include "circle_fit.h"

#define LIBNAME		"circlefit"
#define LIBVERSION	LIBNAME " library for " LUA_VERSION

static int lua_circlefit_fit(lua_State *L){
	int niters, n, stat, i;
	double tol, c[2], r;
	double *pt;
	luaL_argcheck(L, lua_istable(L, 1), 1, "Expected list of points");
	niters = luaL_optinteger(L, 2, 100);
	tol = luaL_optnumber(L, 3, 1e-10);
	
	lua_len(L, 1);
	n = lua_tointeger(L, -1);
	lua_pop(L, 1);
	
	luaL_argcheck(L, n >= 3, 1, "Must specify at least 3 points");
	
	pt = (double*)malloc(sizeof(double) * 2 * n);
	for(i = 0; i < n; ++i){
		int ncoord, j;
		lua_pushinteger(L, i+1);
		lua_gettable(L, 1);
		luaL_argcheck(L, lua_istable(L, -1), 1, "Invalid format for points");
		
		lua_len(L, -1);
		luaL_argcheck(L, 2 == lua_tointeger(L, -1), 1, "Invalid format for points");
		lua_pop(L, 1);
		
		for(j = 0; j < 2; ++j){
			lua_pushinteger(L, j+1);
			lua_gettable(L, -2);
			luaL_argcheck(L, lua_isnumber(L, -1), 1, "Non-numeric point encountered");
			pt[2*i+j] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	stat = circle_fit(n, pt, &niters, &tol, c, &r);
	free(pt);
	
	lua_createtable(L, 2, 0);
	for(i = 0; i < 2; ++i){
		lua_pushinteger(L, i+1);
		lua_pushnumber(L, c[i]);
		lua_settable(L, -3);
	}
	lua_pushnumber(L, r);
	return 2;
}

int luaopen_circlefit(lua_State *L){
	static const luaL_Reg circlefit_lib[] = {
		{"fit", lua_circlefit_fit},
		{NULL, NULL}
	};
	luaL_newlib(L, circlefit_lib);

	return 1;
}
