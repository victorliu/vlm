INCDIR = .
SRCDIR = .

include Makefile.plat

ALL_BINARY_MODULES = \
	bits/bits.$(SHLIB_EXT) \
	dxf/DXF.$(SHLIB_EXT) \
	gpc/GPC.$(SHLIB_EXT) \
	nlopt/NLopt.$(SHLIB_EXT) \
	circlefit/circlefit.$(SHLIB_EXT) \
	random/random.$(SHLIB_EXT)

all: lua_compat.o $(ALL_BINARY_MODULES)

lua_compat.o: lua_compat.c
	$(CC) -c $(CFLAGS) $(LUA_INCLUDE) -O3 $< -o $@

bits/bits.$(SHLIB_EXT): bits/bits.c lua_compat.o
	$(CC) $(CFLAGS) $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< lua_compat.o -o $@ $(LUA_MODULE_LIB)
	
dxf/DXF.$(SHLIB_EXT): dxf/DXF.c lua_compat.o
	$(CC) $(CFLAGS) -Idxf $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< lua_compat.o -o $@ $(LUA_MODULE_LIB)
	
gpc/GPC.$(SHLIB_EXT): gpc/gpc.c gpc/luagpc.c lua_compat.o
	$(CC) $(CFLAGS) -Igpc $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< lua_compat.o gpc/luagpc.c -o $@ $(LUA_MODULE_LIB)
	
nlopt/NLopt.$(SHLIB_EXT): nlopt/LuaNLopt.cpp
	$(CXX) $(CXXFLAGS) $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< lua_compat.o -o $@ $(LUA_MODULE_LIB) -lnlopt

random/random.$(SHLIB_EXT): random/random.cpp
	$(CXX) $(CXXFLAGS) $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< lua_compat.o -o $@ $(LUA_MODULE_LIB)

circlefit/circlefit.$(SHLIB_EXT): circlefit/circle_fit.c circlefit/circle_fit_lua.c lua_compat.o
	$(CC) $(CFLAGS) $(LUA_INCLUDE) -O3 $(SHLIB_FLAGS) $< circlefit/circle_fit_lua.c lua_compat.o -o $@ $(LUA_MODULE_LIB)

clean:
	rm -f bits/bits.$(SHLIB_EXT)
	rm -f dxf/DXF.$(SHLIB_EXT)
	rm -f gpc/GPC.$(SHLIB_EXT)
	rm -f nlopt/nlopt.$(SHLIB_EXT)
	rm -f circlefit/circlefit.$(SHLIB_EXT)

install:
	install bits/bits.$(SHLIB_EXT)           $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install dxf/DXF.$(SHLIB_EXT)             $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install gpc/GPC.$(SHLIB_EXT)             $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install nlopt/nlopt.$(SHLIB_EXT)         $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install random/random.$(SHLIB_EXT)       $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install circlefit/circlefit.$(SHLIB_EXT) $(MODULE_ROOT)/lib/lua/$(LUA_VERSION)/
	install polygonutils/polygonutils.lua    $(MODULE_ROOT)/share/lua/$(LUA_VERSION)/
	install table_stats/table_stats.lua      $(MODULE_ROOT)/share/lua/$(LUA_VERSION)/
install2:
	install bits/bits.$(SHLIB_EXT)           $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install dxf/DXF.$(SHLIB_EXT)             $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install gpc/GPC.$(SHLIB_EXT)             $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install nlopt/nlopt.$(SHLIB_EXT)         $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install random/random.$(SHLIB_EXT)       $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install circlefit/circlefit.$(SHLIB_EXT) $(MODULE_ROOT2)/lib/lua/$(LUA_VERSION)/
	install polygonutils/polygonutils.lua    $(MODULE_ROOT2)/share/lua/$(LUA_VERSION)/
	install table_stats/table_stats.lua      $(MODULE_ROOT2)/share/lua/$(LUA_VERSION)/
	