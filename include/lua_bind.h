#ifndef CRUNUM_LUA
#define CRUNUM_LUA

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

#include "crunum.h"

extern const luaL_Reg matrix_methods[];
extern const luaL_Reg matrix_functions[];
extern const luaL_Reg vector_methods[];
extern const luaL_Reg vector_functions[];

#endif
