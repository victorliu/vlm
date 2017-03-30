#include <ctime>
#include <cmath>
#include <random>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LIBNAME		"random"
#define LIBVERSION	LIBNAME " library for " LUA_VERSION
static const char* random_context_metaname = "RandomContext";

struct lua_random_context{
	std::mt19937 mt;
};
struct lua_random_context g_ctx;

static struct lua_random_context* random_context_check(lua_State *L, int narg){
	luaL_checktype(L, narg, LUA_TUSERDATA);
	void *ud = luaL_checkudata(L, narg, random_context_metaname);
	if(!ud){
		luaL_argerror(L, narg, "expected RandomContext");
		return NULL;
	}
	return (struct lua_random_context*)ud;
}

static int new_context(lua_State *L){
	int ref = LUA_NOREF;
	int seed = luaL_optinteger(L, 1, time(NULL));
	
	struct lua_random_context *c = (struct lua_random_context*)lua_newuserdata(L, sizeof(struct lua_random_context));
	c->mt.seed(seed);
	luaL_getmetatable(L, random_context_metaname);
	lua_setmetatable(L, -2);
	return 1;
}

static int context__gc(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	return 0;
}
static int context__tostring(lua_State *L){
	lua_pushstring(L, random_context_metaname);
	return 1;
}

static int global_seed(lua_State *L){
	lua_Integer a = luaL_checkinteger(L, 1);
	g_ctx.mt.seed(a);
	return 0;
}

static int context_uniform_int(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Integer a = luaL_checkinteger(L, 2);
	lua_Integer b = luaL_checkinteger(L, 3);
	std::uniform_int_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(ctx->mt));
	return 1;
}
static int global_uniform_int(lua_State *L){
	lua_Integer a = luaL_checkinteger(L, 1);
	lua_Integer b = luaL_checkinteger(L, 2);
	std::uniform_int_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(g_ctx.mt));
	return 1;
}

static int context_uniform(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::uniform_real_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_uniform(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::uniform_real_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_normal(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number mean = luaL_checknumber(L, 2);
	lua_Number stdev = luaL_checknumber(L, 3);
	std::normal_distribution<lua_Number> dist(mean, stdev);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_normal(lua_State *L){
	lua_Number mean = luaL_checknumber(L, 1);
	lua_Number stdev = luaL_checknumber(L, 2);
	std::normal_distribution<lua_Number> dist(mean, stdev);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_binomial(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Integer a = luaL_checkinteger(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::binomial_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(ctx->mt));
	return 1;
}
static int global_binomial(lua_State *L){
	lua_Integer a = luaL_checkinteger(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::binomial_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(g_ctx.mt));
	return 1;
}

static int context_negative_binomial(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Integer a = luaL_checkinteger(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::negative_binomial_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(ctx->mt));
	return 1;
}
static int global_negative_binomial(lua_State *L){
	lua_Integer a = luaL_checkinteger(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::negative_binomial_distribution<lua_Integer> dist(a, b);
	lua_pushinteger(L, dist(g_ctx.mt));
	return 1;
}

static int context_bernoulli(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 3);
	std::bernoulli_distribution dist(a);
	lua_pushboolean(L, dist(ctx->mt));
	return 1;
}
static int global_bernoulli(lua_State *L){
	lua_Number a = luaL_checknumber(L, 2);
	std::bernoulli_distribution dist(a);
	lua_pushboolean(L, dist(g_ctx.mt));
	return 1;
}

static int context_cauchy(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::cauchy_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_cauchy(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::cauchy_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_chi_squared(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	std::chi_squared_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_chi_squared(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	std::chi_squared_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_poisson(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	std::poisson_distribution<lua_Integer> dist(a);
	lua_pushinteger(L, dist(ctx->mt));
	return 1;
}
static int global_poisson(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	std::poisson_distribution<lua_Integer> dist(a);
	lua_pushinteger(L, dist(g_ctx.mt));
	return 1;
}

static int context_student_t(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	std::student_t_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_student_t(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	std::student_t_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_gamma(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::gamma_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_gamma(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::gamma_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_geometric(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	std::geometric_distribution<lua_Integer> dist(a);
	lua_pushinteger(L, dist(ctx->mt));
	return 1;
}
static int global_geometric(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	std::geometric_distribution<lua_Integer> dist(a);
	lua_pushinteger(L, dist(g_ctx.mt));
	return 1;
}

static int context_lognormal(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::lognormal_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_lognormal(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::lognormal_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_exponential(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	std::exponential_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_exponential(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	std::exponential_distribution<lua_Number> dist(a);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_extreme_value(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::extreme_value_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_extreme_value(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::extreme_value_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_fisher_f(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::fisher_f_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_fisher_f(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::fisher_f_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_weibull(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	lua_Number a = luaL_checknumber(L, 2);
	lua_Number b = luaL_checknumber(L, 3);
	std::weibull_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(ctx->mt));
	return 1;
}
static int global_weibull(lua_State *L){
	lua_Number a = luaL_checknumber(L, 1);
	lua_Number b = luaL_checknumber(L, 2);
	std::weibull_distribution<lua_Number> dist(a, b);
	lua_pushnumber(L, dist(g_ctx.mt));
	return 1;
}

static int context_uniform_disk(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	std::uniform_real_distribution<lua_Number> dist(0, 1);
	lua_Number a = dist(ctx->mt);
	lua_Number b = dist(ctx->mt);
	if(b < a){ std::swap(a, b); }
	lua_pushnumber(L, b*cos(2*M_PI*a));
	lua_pushnumber(L, b*sin(2*M_PI*a));
	return 2;
}
static int global_uniform_disk(lua_State *L){
	std::uniform_real_distribution<lua_Number> dist(0, 1);
	lua_Number a = dist(g_ctx.mt);
	lua_Number b = dist(g_ctx.mt);
	if(b < a){ std::swap(a, b); }
	lua_pushnumber(L, b*cos(2*M_PI*a));
	lua_pushnumber(L, b*sin(2*M_PI*a));
	return 2;
}

static int context_uniform_sphere(lua_State *L){
	struct lua_random_context *ctx = random_context_check(L, 1);
	std::uniform_real_distribution<lua_Number> dist(0, 1);
	lua_Number a = dist(ctx->mt);
	lua_Number z = dist(ctx->mt);
	lua_Number r = sqrt((1+z)*(1-z));
	lua_pushnumber(L, r*cos(2*M_PI*a));
	lua_pushnumber(L, r*sin(2*M_PI*a));
	lua_pushnumber(L, z);
	return 2;
}
static int global_uniform_sphere(lua_State *L){
	std::uniform_real_distribution<lua_Number> dist(0, 1);
	lua_Number a = dist(g_ctx.mt);
	lua_Number z = dist(g_ctx.mt);
	lua_Number r = sqrt((1+z)*(1-z));
	lua_pushnumber(L, r*cos(2*M_PI*a));
	lua_pushnumber(L, r*sin(2*M_PI*a));
	lua_pushnumber(L, z);
	return 2;
}

extern "C"
int luaopen_random(lua_State *L){
	static const luaL_Reg context_methods[] = {
		{"uniform_int", context_uniform_int},
		{"uniform", context_uniform},
		{"normal", context_normal},
		{"binomial", context_binomial},
		{"negative_binomial", context_negative_binomial},
		{"bernoulli", context_bernoulli},
		{"cauchy", context_cauchy},
		{"chi_squared", context_chi_squared},
		{"poisson", context_poisson},
		{"student_t", context_student_t},
		{"gamma", context_gamma},
		{"geometric", context_geometric},
		{"lognormal", context_lognormal},
		{"exponential", context_exponential},
		{"extreme_value", context_extreme_value},
		{"fisher_f", context_fisher_f},
		{"weibull", context_weibull},
		/*
		{"discrete", context_discrete},
		{"piecewise_constant", context_piecewise_constant},
		{"piecewise_linear", context_piecewise_linear},
		*/
		{"uniform_sphere", context_uniform_sphere},
		{"uniform_disk", context_uniform_disk},
		{NULL, NULL}
	};
	static const luaL_Reg context_metamethods[] = {
		{"__gc", context__gc},
		{"__tostring", context__tostring},
		{NULL, NULL}
	};
	const int stackTest = lua_gettop(L);

    if(0 == luaL_newmetatable(L, random_context_metaname)){
		return luaL_error(L, "metatable '%s' already registered", random_context_metaname);
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

	for(const luaL_Reg* l = context_metamethods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, metatable);
	}
	
	for(const luaL_Reg* l = context_methods; NULL != l && l->name; l++){
		lua_pushstring(L, l->name);
		lua_pushcfunction(L, l->func);
		lua_rawset(L, methodtable);
	}

	lua_pop(L, 2);  // metatable, methodtable
	
	static const luaL_Reg random_lib[] = {
		{"new_generator", new_context},
		{"seed", global_seed},
		{"uniform_int", global_uniform_int},
		{"uniform", global_uniform},
		{"normal", global_normal},
		{"binomial", global_binomial},
		{"negative_binomial", global_negative_binomial},
		{"bernoulli", global_bernoulli},
		{"cauchy", global_cauchy},
		{"chi_squared", global_chi_squared},
		{"poisson", global_poisson},
		{"student_t", global_student_t},
		{"gamma", global_gamma},
		{"geometric", global_geometric},
		{"lognormal", global_lognormal},
		{"exponential", global_exponential},
		{"extreme_value", global_extreme_value},
		{"fisher_f", global_fisher_f},
		{"weibull", global_weibull},
		/*
		{"discrete", global_discrete},
		{"piecewise_constant", global_piecewise_constant},
		{"piecewise_linear", global_piecewise_linear},
		*/
		{"uniform_sphere", global_uniform_sphere},
		{"uniform_disk", global_uniform_disk},
		{NULL, NULL}
	};
	luaL_newlib(L, random_lib);

	return 1;
}
