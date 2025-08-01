/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#pragma message "Lua Crunum"

#include "matrix.h"
#include "vector.h"

extern const luaL_Reg matrix_methods[];
extern const luaL_Reg matrix_functions[];
extern const luaL_Reg vector_methods[];
extern const luaL_Reg vector_functions[];

int luaopen_crunum(lua_State* lua){
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
	return 1;
}
