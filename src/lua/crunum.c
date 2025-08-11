/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#pragma message "Lua Crunum"

#include "config.h"

#include <stdlib.h>
#include <time.h>

#include "lua_bind.h"

int luaopen_crunum(lua_State* lua){
	srand(time(NULL));
	luaL_newmetatable(lua, "CrunumMatrix");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");
	luaL_setfuncs(lua, matrix_methods, 0);
	luaL_newmetatable(lua, "CrunumVector");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");
	luaL_setfuncs(lua, vector_methods, 0);
	lua_newtable(lua);
	luaL_setfuncs(lua, matrix_functions, 0);
	lua_setfield(lua, -2, "matrix");
	lua_newtable(lua);
	luaL_setfuncs(lua, vector_functions, 0);
	lua_setfield(lua, -2, "vector");
	lua_pushstring(lua, VERSION);
	lua_setfield(lua, -2, "__version__");
	return 1;
}
