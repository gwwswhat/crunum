/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under the GPL-3.0 License. See LICENSE for details.
 */

#pragma message "Lua Vector"

#include "lua_bind.h"

static int l_vector_new(lua_State* lua){
	int len = luaL_checkinteger(lua, 1);
	if(len < 0){
		luaL_error(lua, "Vector length can't be negative");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = vector_new((uint)len, (float)luaL_optnumber(lua, 2, 0));
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_vector_randinit(lua_State* lua){
	int len = luaL_checkinteger(lua, 1);
	if(len < 0){
		luaL_error(lua, "Vector length can't be negative");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = vector_randinit((uint)len);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_vector_from(lua_State* lua){
	luaL_checktype(lua, 1, LUA_TTABLE);
	uint len = lua_rawlen(lua, 1);
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = vector_new(len, 0);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	for(uint i = 0; i < len; i++){
		lua_rawgeti(lua, 1, i + 1);
		(*vector)->values[i] = luaL_checkinteger(lua, -1);
		lua_pop(lua, 1);
	}
	return 1;
}

static int l_vector_len(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	lua_pushinteger(lua, vector->len);
	return 1;
}

static int l_vector_push(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	float value = luaL_checknumber(lua, 2);
	vector_push(vector, value);
	return 0;
}

static int l_vector_pop(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	if(!vector->len){
		luaL_error(lua, "Empty vector");
		return 0;
	}
	lua_pushnumber(lua, vector_pop(vector));
	return 1;
}

static int l_vector_index(lua_State* lua){
	int type = lua_type(lua, 2);
	if(type == LUA_TSTRING){
		luaL_getmetatable(lua, "CrunumVector");
		lua_pushvalue(lua, 2);
		lua_rawget(lua, -2);
		return 1;
	}
	if(type == LUA_TNUMBER){
		struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
		int index = luaL_checkinteger(lua, 2) - 1;
		if((uint)index > vector->len || index < 0){
			luaL_error(lua, "Out of bound");
			return 0;
		}
		lua_pushnumber(lua, vector->values[index]);
		return 1;
	}
	luaL_error(lua, "Invalid __index value");
	return 1;
}

static int l_vector_newindex(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	int index = luaL_checkinteger(lua, 2) - 1;
	if((uint)index >= vector->len || index < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	float value = luaL_checknumber(lua, 3);
	vector->values[index] = value;
	return 0;
}

static int l_vector_gc(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	vector_free(vector);
	return 1;
}

static int l_vector_tostring(lua_State* lua){
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	luaL_Buffer buffer;
	luaL_buffinit(lua, &buffer);
	luaL_addchar(&buffer, '{');
	for(uint i = 0; i < vector->len; i++){
		char num[16];
		snprintf(num, sizeof(num), "%.2lf", vector->values[i]);
		luaL_addstring(&buffer, num);
		if(i != vector->len - 1)
			luaL_addstring(&buffer, ", ");
	}
	luaL_addchar(&buffer, '}');
	luaL_pushresult(&buffer);
	return 1;
}

static int l_vector_mul(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector** vector2 = luaL_testudata(lua, 2, "CrunumVector");
	if(vector2){
		if(vector1->len != (*vector2)->len){
			luaL_error(lua, "Vector length doesn't match another vector length");
			return 0;
		}
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_mul(vector1, *vector2);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Matrix** matrix = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix){
		if(vector1->len != (*matrix)->rows){
			luaL_error(lua, "Vector length doesn't match matrix row size");
			return 0;
		}
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_mul_matrix(vector1, *matrix);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_mul_scalar(vector1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either Vector, Matrix or Scalar");
	return 0;
}

static int l_vector_add(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector** vector2 = luaL_testudata(lua, 2, "CrunumVector");
	if(vector2){
		if(vector1->len != (*vector2)->len){
			luaL_error(lua, "Vector length doesn't match another vector length");
			return 0;
		}
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_add(vector1, *vector2);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_add_scalar(vector1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either Vector or Scalar");
	return 0;
}

static int l_vector_eq(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_eq(vector1, vector2));
	return 1;
}

static int l_vector_neq(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_neq(vector1, vector2));
	return 1;
}

static int l_vector_gt(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_gt(vector1, vector2));
	return 1;
}

static int l_vector_ge(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_ge(vector1, vector2));
	return 1;
}

static int l_vector_lt(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_lt(vector1, vector2));
	return 1;
}

static int l_vector_le(lua_State* lua){
	struct Vector* vector1 = *(struct Vector**)luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector* vector2 = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	if(vector1->len != vector2->len){
		luaL_error(lua, "Both vector aren't the same length");
		return 0;
	}
	lua_pushboolean(lua, (int)vector_le(vector1, vector2));
	return 1;
}

const luaL_Reg vector_functions[] = {
	{"new", l_vector_new},
	{"randinit", l_vector_randinit},
	{"from", l_vector_from},
	{NULL, NULL}
};

const luaL_Reg vector_methods[] = {
	{"len", l_vector_len},
	{"push", l_vector_push},
	{"pop", l_vector_pop},
	{"__index", l_vector_index},
	{"__newindex", l_vector_newindex},
	{"__gc", l_vector_gc},
	{"__tostring", l_vector_tostring},
	{"__mul", l_vector_mul},
	{"__add", l_vector_add},
	{"__eq", l_vector_eq},
	{"__neq", l_vector_neq},
	{"__gt", l_vector_gt},
	{"__ge", l_vector_ge},
	{"__lt", l_vector_lt},
	{"__le", l_vector_le},
	{NULL, NULL}
};
