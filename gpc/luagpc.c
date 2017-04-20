#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "../lua_compat.h"
#include "gpc.h"

#define LIBNAME		"GPC"
#define LIBVERSION	LIBNAME " library for " LUA_VERSION

static void poly_init(gpc_polygon *p){
	p->num_contours = 0;
	p->hole = NULL;
	p->contour = NULL;
}

static int lua_checkvlist(lua_State *L, int ivlist, gpc_vertex_list *vlist){
	size_t j;
	if(ivlist < 0){ ivlist += 1 + lua_gettop(L); }
	
	size_t n = lcl_len(L, ivlist);
	
	vlist->num_vertices = (int)n;
	vlist->vertex = (gpc_vertex*)realloc(vlist->vertex, n*sizeof(gpc_vertex));
	for(j = 0; j < n; ++j){ // for each vertex
		lua_pushinteger(L, j+1);
		lua_gettable(L, ivlist);
		
		if(!lua_istable(L, -1) || 2 != lcl_len(L, -1)){
			lua_pop(L, 1);
			return 0;
		};
		
		lua_pushinteger(L, 1);
		lua_gettable(L, -2);
		if(!lua_isnumber(L, -1)){
			lua_pop(L, 2);
			return 0;
		}
		vlist->vertex[j].x = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_pushinteger(L, 2);
		lua_gettable(L, -2);
		if(!lua_isnumber(L, -1)){
			lua_pop(L, 2);
			return 0;
		}
		vlist->vertex[j].y = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_pop(L, 1);
	}
	return 1;
}

static int lua_checkpoly(lua_State *L, int ipoly, gpc_polygon *p){
	size_t i;
	unsigned j;
	gpc_vertex_list vlist;
	int *hole;
	if(!lua_istable(L, ipoly)){ return 0; }
	size_t n = lcl_len(L, ipoly);
	
	// Initialize vertex and hole lists
	vlist.num_vertices = 0;
	vlist.vertex = NULL;
	hole = (int*)malloc(n*sizeof(int));
	for(i = 0; i < n; ++i){
		hole[i] = 0;
	}
	
	// check if there is a hole record
	lua_getfield(L, ipoly, "hole");
	if(!lua_isnil(L, -1)){
		size_t nh = lcl_len(L, -1);
		for(i = 0; i < nh; ++i){
			lua_pushinteger(L, i+1);
			lua_gettable(L, -2);
			if(lua_isboolean(L, -1)){
				hole[i] = lua_toboolean(L, -1);
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	int single_poly = 0;
	for(i = 0; i < n; ++i){ // for each contour
		lua_pushinteger(L, i+1);
		lua_gettable(L, ipoly);
		
		if(!lua_istable(L, -1)){
			lua_pop(L, 1);
			return 0;
		};
		
		unsigned nh = lcl_len(L, -1);
		
		if(2 == nh && 0 == i){
			lua_pushinteger(L, 1);
			lua_gettable(L, -2);
			if(lua_isnumber(L, -1)){
				single_poly = 1;
				lua_pop(L, 2);
				break;
			}
			lua_pop(L, 1);
		}
		
		if(!lua_checkvlist(L, -1, &vlist)){
			return luaL_error(L, "Invalid vertex list at index %d", (int)(i+1));
		}
		lua_pop(L, 1);
		
		gpc_add_contour(p, &vlist, hole[i]);
	}
	if(single_poly){
		if(!lua_checkvlist(L, ipoly, &vlist)){
			return luaL_argerror(L, ipoly, "Invalid vertex list for single contour polygon");
		}
		
		gpc_add_contour(p, &vlist, 0);
	}
	
	free(hole);
	free(vlist.vertex);
	return 1;
}

static int lua_pushvlist(lua_State *L, const gpc_vertex_list *vlist){
	int i;
	lua_createtable(L, vlist->num_vertices, 0);
	for(i = 0; i < vlist->num_vertices; ++i){
		lua_pushinteger(L, i+1);
		lua_createtable(L, 2, 0);
		
		lua_pushinteger(L, 1);
		lua_pushnumber(L, vlist->vertex[i].x);
		lua_settable(L, -3);
		
		lua_pushinteger(L, 2);
		lua_pushnumber(L, vlist->vertex[i].y);
		lua_settable(L, -3);
		
		lua_settable(L, -3);
	}
	return 1;
}

static int lua_pushpoly(lua_State *L, const gpc_polygon *p){
	int i;
	if(p->num_contours == 0){
		lua_createtable(L, 0, 0);
		return 1;
	}/*else if(p->num_contours == 1){
		lua_pushvlist(L, &(p->contour[0]));
		return 1;
	}*/else{
		lua_createtable(L, p->num_contours, 1);
		lua_createtable(L, p->num_contours, 0);
		for(i = 0; i < p->num_contours; ++i){
			lua_pushinteger(L, i+1);
			lua_pushvlist(L, &(p->contour[i]));
			lua_settable(L, -4);
			
			lua_pushinteger(L, i+1);
			lua_pushboolean(L, p->hole[i]);
			lua_settable(L, -3);
		}
		lua_setfield(L, -2, "hole");
		return 1;
	}
}

static int gpc_operation(lua_State *L, gpc_op op){
	int j;
	int n = lua_gettop(L);
	if(n < 2){ return n; }
	gpc_polygon p0, p1, p2;
	poly_init(&p0);
	poly_init(&p2);
	if(!lua_checkpoly(L, 1, &p0)){ return luaL_argerror(L, 1, "Expected polygon"); }
	for(j = 1; j < n; ++j){
		poly_init(&p1);
		if(!lua_checkpoly(L, j+1, &p1)){ return luaL_argerror(L, j+1, "Expected polygon"); }
		gpc_polygon_clip(op, &p0, &p1, &p2);
		gpc_free_polygon(&p0);
		gpc_free_polygon(&p1);
		if(0 == p2.num_contours){ break; }
		p0 = p2;
	}
	lua_pushpoly(L, &p2);
	gpc_free_polygon(&p2);
	return 1;
}

static int gpc_union(lua_State *L){ return gpc_operation(L, GPC_UNION); }
static int gpc_intersection(lua_State *L){ return gpc_operation(L, GPC_INT); }
static int gpc_difference(lua_State *L){ return gpc_operation(L, GPC_DIFF); }
static int gpc_xor(lua_State *L){ return gpc_operation(L, GPC_XOR); }
static int gpc_totristrip(lua_State *L){
	int i;
	gpc_polygon p;
	gpc_tristrip trs;
	poly_init(&p);
	if(!lua_checkpoly(L, 1, &p)){ return luaL_argerror(L, 1, "Expected polygon"); }
	gpc_polygon_to_tristrip(&p, &trs);
	
	lua_createtable(L, trs.num_strips, 1);
	for(i = 0; i < trs.num_strips; ++i){
		lua_pushinteger(L, i+1);
		lua_pushvlist(L, &(trs.strip[i]));
		lua_settable(L, -3);
	}
	return 1;
	
	gpc_free_tristrip(&trs);
	gpc_free_polygon(&p);
}

int luaopen_GPC(lua_State *L){
	static const luaL_Reg GPC_lib[] = {
		{"difference", gpc_difference },
		{"intersection", gpc_intersection },
		{"xor", gpc_xor },
		{"union", gpc_union },
		{"tristrip", gpc_totristrip },
		{NULL, NULL}
	};
	luaL_newlib(L, GPC_lib);

	return 1;
}
