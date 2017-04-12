#include <stdio.h>
#include <math.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define LIBNAME		"DXF"
#define LIBVERSION	LIBNAME " library for " LUA_VERSION
static const char* writer_metaname = "DXFWriter";

struct lua_dxf_writer{
	FILE *f;
	int current_layer;
	int stream_ref;
	int f_needs_closing;
	int EOF_written;
};

static struct lua_dxf_writer* writer_check(lua_State *L, int narg){
	luaL_checktype(L, narg, LUA_TUSERDATA);
	void *ud = luaL_checkudata(L, narg, writer_metaname);
	if(!ud){
		luaL_argerror(L, narg, "expected DXFWriter");
		return NULL;
	}
	return (struct lua_dxf_writer*)ud;
}

static int new_writer(lua_State *L){
	int ref = LUA_NOREF;
	FILE *fp = stdout;
	int f_needs_closing = 0;
	
	luaL_argcheck(L, lua_istable(L, 1), 1, "Must use named arguments");
	
	lua_getfield(L, 1, "stream");
	if(!lua_isnil(L, -1)){
		luaL_Stream *s = luaL_checkudata(L, -1, LUA_FILEHANDLE);
		ref = luaL_ref(L, LUA_REGISTRYINDEX);
		fp = s->f;
	}else{
		lua_pop(L, 1);
	}
	
	lua_getfield(L, 1, "filename");
	if(!lua_isnil(L, -1)){
		fp = fopen(lua_tostring(L, -1), "wb");
		f_needs_closing = 1;
	}
	lua_pop(L, 1);
	
	struct lua_dxf_writer *p = (struct lua_dxf_writer*)lua_newuserdata(L, sizeof(struct lua_dxf_writer));
	p->f = fp;
	p->current_layer = 0;
	p->stream_ref = ref;
	p->f_needs_closing = f_needs_closing;
	p->EOF_written = 0;
	luaL_getmetatable(L, writer_metaname);
	lua_setmetatable(L, -2);
	
	fprintf(fp,
		"0\n"
		"SECTION\n"
		"2\n"
		"HEADER\n"
		"9\n"
		"$ACADVER\n"
		"1\n"
		"AC1006\n"
		"0\n"
		"ENDSEC\n"
		
		"0\n"
		"SECTION\n"
		"2\n"
		"TABLES\n"
	);
	
	lua_getfield(L, 1, "layers");
	if(!lua_isnoneornil(L, -1)){
		int i, n;
		lua_len(L, -1);
		n = lua_tointeger(L, -1);
		lua_pop(L, 1);
		
		fprintf(fp,
			"0\n"
			"TABLE\n"
			"2\n"
			"LAYER\n"
			"70\n"
			"%d\n",
			n
		);
		for(i = 0; i < n; ++i){
			const char *layer_name;
			lua_pushinteger(L, i+1);
			lua_gettable(L, -2);
			layer_name = lua_tostring(L, -1);
			
			fprintf(fp,
				"0\n"
				"LAYER\n"
				"70\n"
				"0\n"
				"62\n"
				"%d\n"
				"6\n"
				"CONTINUOUS\n"
				"2\n"
				"%s\n",
				i+1, layer_name
			);
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	fprintf(fp,
		"0\n"
		"ENDTAB\n"
		"0\n"
		"ENDSEC\n"
		
		// Blocks
		"0\n"
		"SECTION\n"
		"2\n"
		"BLOCKS\n"
		"0\n"
		"ENDSEC\n"
		
		// Entities
		"0\n"
		"SECTION\n"
		"2\n"
		"ENTITIES\n"
	);
	return 1;
}

static int writer__gc(lua_State *L){
	struct lua_dxf_writer *writer = writer_check(L, 1);
	if(!writer->EOF_written){
		fprintf(writer->f,
			"0\n"
			"ENDSEC\n"
			"0\n"
			"EOF\n"
		);
		fflush(writer->f);
	}
	if(writer->f_needs_closing){
		fclose(writer->f);
	}
	luaL_unref(L, LUA_REGISTRYINDEX, writer->stream_ref);
	return 0;
}
static int writer__tostring(lua_State *L){
	lua_pushstring(L, writer_metaname);
	return 1;
}
static int writer_set_layer(lua_State *L){
	struct lua_dxf_writer *writer = writer_check(L, 1);
	int i = luaL_checkinteger(L, 2);
	lua_pushinteger(L, writer->current_layer);
	writer->current_layer = i;
	return 1;
}
static int writer_circle(lua_State *L){
	double cx = 0;
	double cy = 0;
	double r = 0;
	struct lua_dxf_writer *writer = writer_check(L, 1);
	luaL_argcheck(L, lua_istable(L, 2), 2, "Must use named arguments");
	
	lua_getfield(L, 2, "center");
	if(lua_istable(L, -1)){
		lua_pushinteger(L, 1);
		lua_gettable(L, -2);
		cx = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_pushinteger(L, 2);
		lua_gettable(L, -2);
		cy = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	
	lua_getfield(L, 2, "radius");
	if(lua_isnumber(L, -1)){
		r = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	
	fprintf(writer->f,
		"0\n"
		"CIRCLE\n"
		"8\n"
		"%d\n"
		"10\n"
		"%f\n"
		"20\n"
		"%f\n"
		"40\n"
		"%f\n",
		writer->current_layer, cx, cy, r
	);
	return 0;
}
static int writer_polyline(lua_State *L){
	int i, n;
	struct lua_dxf_writer *writer = writer_check(L, 1);
	luaL_argcheck(L, lua_istable(L, 2), 2, "Must provide a table of vertices");
	
	lua_len(L, 2);
	n = lua_tointeger(L, -1);
	lua_pop(L, 1);
	
	fprintf(writer->f,
		"0\n"
		"POLYLINE\n"
		"8\n"
		"%d\n"
		"70\n"
		"1\n" /* closed or open */
		"40\n"
		"0\n"
		"41\n"
		"0\n"
		"66\n"
		"1\n",
		writer->current_layer
	);
	for(i = 0; i < n; ++i){
		double x, y;
		lua_pushinteger(L, i+1);
		lua_gettable(L, 2);
		
		if(!lua_istable(L, -1)){
			return luaL_argerror(L, 2, "Error in table of vertices");
		}
		
		lua_pushinteger(L, 1);
		lua_gettable(L, -2);
		x = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_pushinteger(L, 2);
		lua_gettable(L, -2);
		y = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		fprintf(writer->f,
			"0\n"
			"VERTEX\n"
			"8\n"
			"%d\n"
			"10\n"
			"%f\n"
			"20\n"
			"%f\n",
			writer->current_layer, x, y
		);
		
		lua_pop(L, 1);
	}
	fprintf(writer->f,
		"0\n"
		"SEQEND\n"
	);
	
	return 0;
}

int luaopen_DXF(lua_State *L){
	const luaL_Reg* l;
	static const luaL_Reg writer_methods[] = {
		{"set_layer", writer_set_layer},
		{"circle", writer_circle},
		{"polyline", writer_polyline},
		{NULL, NULL}
	};
	static const luaL_Reg writer_metamethods[] = {
		{"__gc", writer__gc},
		{"__tostring", writer__tostring},
		{NULL, NULL}
	};
	const int stackTest = lua_gettop(L);

    if(0 == luaL_newmetatable(L, writer_metaname)){
		return luaL_error(L, "metatable '%s' already registered", writer_metaname);
	}
	const int metatable = lua_gettop(L);
	
	lua_newtable(L);
	const int methodtable = lua_gettop(L);

	// Translate attribute __metatable of meta. This is the value that is
	// returned by getmetatable(obj). Trick to hide real metatable.
    // Does not work if debug.getmetatable is called.
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methodtable );
	lua_settable(L, metatable);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, methodtable);
	lua_settable(L, metatable);

	for(l = writer_metamethods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, metatable);
	}
	
	for(l = writer_methods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, methodtable);
	}

	lua_pop(L, 2);  // metatable, methodtable
	
	static const luaL_Reg DXF_lib[] = {
		{"new_writer", new_writer},
		{NULL, NULL}
	};
	luaL_newlib(L, DXF_lib);

	return 1;
}
