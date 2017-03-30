#include <stdio.h>
#include <math.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define LIBNAME		"bits"
#define LIBVERSION	LIBNAME " library for " LUA_VERSION
static const char* intpacker_metaname = "intpacker";

static const unsigned int mask[33] = {
	0x0000,
	0x0001, 0x0003, 0x0007, 0x000F,
	0x001F, 0x003F, 0x007F, 0x00FF,
	0x01FF, 0x03FF, 0x07FF, 0x0FFF,
	0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
};

struct intpacker{
	int nfields;
	int npos[33];
};

static int bits_tograycode(lua_State *L){
	unsigned int x = luaL_checkinteger(L, 1);
	unsigned int y = (x ^ (x >> 1));
	lua_pushinteger(L, y);
	return 1;
}
static int bits_fromgraycode(lua_State *L){
	unsigned int x = luaL_checkinteger(L, 1);
	x = x ^ (x >> 16);
	x = x ^ (x >> 8);
	x = x ^ (x >> 4);
	x = x ^ (x >> 2);
	x = x ^ (x >> 1);
	lua_pushinteger(L, x);
	return 1;
}

static unsigned blog2(unsigned int v){
	register unsigned int r; // result of log2(v) will go here
	register unsigned int shift;

	r =     (v > 0xFFFF) << 4; v >>= r;
	shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
	shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
											r |= (v >> 1);
	return r;
}
static int bits_log2(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, blog2(x));
	return 1;
}

static unsigned blog10(unsigned int v){
	register unsigned int r; // result of log2(v) will go here
	int t;          // temporary

	static unsigned int const PowersOf10[] = {
		1,
		10,
		100,
		1000,
		10000,
		100000,
		1000000,
		10000000,
		100000000,
		1000000000
	};

	t = (blog2(v) + 1) * 1233 >> 12; // (use a lg2 method from above)
	r = t - (v < PowersOf10[t]);
	return r;
}
static int bits_log10(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, blog10(x));
	return 1;
}
static unsigned int bispow2(unsigned int v, int zero_is_pow2){
	int f;
	if(zero_is_pow2){
		f = (v & (v - 1)) == 0;
	}else{
		f = v && !(v & (v - 1));
	}
	return f;
}

static int bits_ispow2(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	int b = 0;
	if(lua_isboolean(L, 2)){
		b = lua_toboolean(L, 2);
	}
	lua_pushboolean(L, bispow2(x, b));
	return 1;
}

static int bnset(unsigned int v){
	unsigned int c; // store the total here
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
	return c;
}
static int bits_nset(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, bnset(x));
	return 1;
}
static int bparity(unsigned int v){
	v ^= v >> 16;
	v ^= v >> 8;
	v ^= v >> 4;
	v &= 0xf;
	return (0x6996 >> v) & 1;
}
static int bits_parity(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, bparity(x));
	return 1;
}
static unsigned int breverse(unsigned int v){
	// swap odd and even bits
	v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
	// swap consecutive pairs
	v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
	// swap nibbles ... 
	v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
	// swap bytes
	v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
	// swap 2-byte long pairs
	v = ( v >> 16             ) | ( v               << 16);
	return v;
}
static int bits_reverse(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, breverse(x));
	return 1;
}
static unsigned int brzeros(unsigned int v){
	unsigned int c = 32; // c will be the number of zero bits on the right
	v &= -(signed int)(v);
	if (v) c--;
	if (v & 0x0000FFFF) c -= 16;
	if (v & 0x00FF00FF) c -= 8;
	if (v & 0x0F0F0F0F) c -= 4;
	if (v & 0x33333333) c -= 2;
	if (v & 0x55555555) c -= 1;
	return c;
}
static int bits_rzeros(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, brzeros(x));
	return 1;
}

static unsigned int bnextpow2(unsigned int v){
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}
static int bits_nextpow2(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, bnextpow2(x));
	return 1;
}
static unsigned int bnextperm(unsigned int v){
	unsigned int w;
	unsigned int t = v | (v - 1); // t gets v's least significant 0 bits set to 1
	// Next set to 1 the most significant bit to change, 
	// set to 0 the least significant ones, and add the necessary 1 bits.
#ifdef _MSC_VER
	w = (t + 1) | (((~t & -~t) - 1) >> (_BitScanForward(v) + 1));  
#else
	w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));  
#endif
	return w;
}
static int bits_nextperm(lua_State *L){
	int x = luaL_checkinteger(L, 1);
	lua_pushinteger(L, bnextperm(x));
	return 1;
}
		
static struct intpacker* intpacker_check(lua_State *L, int narg){
	luaL_checktype(L, narg, LUA_TUSERDATA);
	void *ud = luaL_checkudata(L, narg, intpacker_metaname);
	if(!ud){
		luaL_argerror(L, narg, "expected intpacker");
		return NULL;
	}
	return (struct intpacker*)ud;
}

static int bits_intpacker(lua_State *L){
	int n = lua_gettop(L);
	int i;
	int next_pos = 0;
	
	if(n > 32){
		return luaL_error(L, "Too many fields");
	}
	
	struct intpacker *p = (struct intpacker*)lua_newuserdata(L, sizeof(struct intpacker));
	
	p->nfields = n;
	for(i = 0; i < n; ++i){
		int maxval = lua_tointeger(L, i+1);
		p->npos[i] = next_pos;
		next_pos += 1+log2(maxval);
		if(next_pos >= 32){
			return luaL_error(L, "Too many bits required");
		}
	}
	p->npos[i] = next_pos;
	
	luaL_getmetatable(L, intpacker_metaname);
	lua_setmetatable(L, -2);
	
	return 1;
}

static int intpacker__gc(lua_State *L){
	struct intpacker *packer = intpacker_check(L, 1);
	return 0;
}
static int intpacker__tostring(lua_State *L){
	lua_pushstring(L, intpacker_metaname);
	return 1;
}
static int intpacker_pack(lua_State *L){
	struct intpacker *packer = intpacker_check(L, 1);
	int n = lua_gettop(L);
	int i;
	int y = 0;
	if(n != packer->nfields+1){
		luaL_error(L, "Expected %d packing arguments", packer->nfields);
	}
	for(int i = 0; i < packer->nfields; ++i){
		int x = lua_tointeger(L, i+2);
		int imask = packer->npos[i+1] - packer->npos[i];
		y |= ((((unsigned int)x) & mask[imask]) << packer->npos[i]);
	}
	lua_pushinteger(L, y);
	return 1;
}
static int intpacker_unpack(lua_State *L){
	struct intpacker *packer = intpacker_check(L, 1);
	unsigned int x = luaL_checkinteger(L, 2);
	int i;
	for(int i = 0; i < packer->nfields; ++i){
		int imask = packer->npos[i+1] - packer->npos[i];
		unsigned int y = x & mask[imask];
		x >>= imask;
		lua_pushinteger(L, y);
	}
	return packer->nfields;
}

int luaopen_bits(lua_State *L){
	static const luaL_Reg intpacker_methods[] = {
		{"pack", intpacker_pack},
		{"unpack", intpacker_unpack},
		{NULL, NULL}
	};
	static const luaL_Reg intpacker_metamethods[] = {
		{"__gc", intpacker__gc},
		{"__tostring", intpacker__tostring},
		{NULL, NULL}
	};
	const int stackTest = lua_gettop(L);

    if(0 == luaL_newmetatable(L, intpacker_metaname)){
		return luaL_error(L, "metatable '%s' already registered", intpacker_metaname);
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

	for(const luaL_Reg* l = intpacker_metamethods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, metatable);
	}
	
	for(const luaL_Reg* l = intpacker_methods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, methodtable);
	}

	lua_pop(L, 2);  // metatable, methodtable
	
	static const luaL_Reg bits_lib[] = {
		{"intpacker", bits_intpacker},
		{"tograycode", bits_tograycode},
		{"fromgraycode", bits_fromgraycode},
		{"log2", bits_log2},
		{"log10", bits_log10},
		{"ispow2", bits_ispow2},
		{"nset", bits_nset},
		{"parity", bits_parity},
		{"reverse", bits_reverse},
		{"rzeros", bits_rzeros},
		{"nextpow2", bits_nextpow2},
		{"nextperm", bits_nextperm},
		{NULL, NULL}
	};
	luaL_newlib(L, bits_lib);

	return 1;
}
