#ifndef LUA_COMPAT_H_INCLUDED
#define LUA_COMPAT_H_INCLUDED

#include <lua.h>

#ifndef LUA_OK
#define LUA_OK 0
#endif

#if LUA_VERSION_NUM > 501
#define LUA_SETFUNCS(L, R) luaL_setfuncs(L, R, 0)
#else
#define LUA_SETFUNCS(L, R) luaL_register(L, NULL, R)
#endif

#ifndef luaL_newlibtable
#define luaL_newlibtable(L,l)	\
  lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)
#endif

#ifndef luaL_newlib
#define luaL_newlib(L,l)  \
  (luaL_newlibtable(L,l), LUA_SETFUNCS(L,l))
#endif

int lcl_len(lua_State *L, int idx);

#endif /* LUA_COMPAT_H_INCLUDED */
