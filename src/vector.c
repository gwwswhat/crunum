#include <stdlib.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

#include "matrix.h"
#include "vector.h"

struct Vector* vector_init(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->values = calloc(1, len * sizeof(double));
	return vector;
}

struct Vector* vector_randinit(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->values = malloc(len * sizeof(double));
	for(uint i = 0; i < len; i++)
		vector->values[i] = (double)rand() / (double)RAND_MAX;
	return vector;
}

struct Vector* vector_from_matrix(struct Matrix* matrix){
	struct Vector* vector = vector_init(matrix->cols);
	for(uint i = 0; i < vector->len; i++)
		vector->values[i] = matrix->values[i];
	return vector;
}

void vector_free(struct Vector* vector){
	free(vector->values);
	free(vector);
}

void vector_push(struct Vector* vector, double value){
	vector->values = realloc(vector->values, ++vector->len * sizeof(double));
	vector->values[vector->len - 1] = value;
}

struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_init(vector1->len);
	for(uint i = 0; i < vector1->len; i++)
		result->values[i] = vector1->values[i] * vector2->values[i];
	return result;
}

struct Vector* vector_mul_scalar(struct Vector* vector, double scalar){
	struct Vector* result = vector_init(vector->len);
	for(uint i = 0; i < vector->len; i++)
		result->values[i] = vector->values[i] * scalar;
	return result;
}

struct Vector* vector_mul_matrix(struct Vector* vector, struct Matrix* matrix){
	struct Vector* result = vector_init(vector->len);
	for(uint i = 0; i < vector->len; i++)
		for(uint j = 0; j < matrix->cols; j++)
			for(uint k = 0; k < matrix->rows; k++)
				result->values[j] += vector->values[i] * matrix_get(matrix, j, k);
	return result;
}

struct Vector* vector_add(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_init(vector1->len);
	for(uint i = 0; i < vector1->len; i++)
		result->values[i] = vector1->values[i] + vector2->values[i];
	return result;
}

struct Vector* vector_add_scalar(struct Vector* vector, double scalar){
	struct Vector* result = vector_init(vector->len);
	for(uint i = 0; i < vector->len; i++)
		result->values[i] = vector->values[i] + scalar;
	return result;
}

static int l_vector_init(lua_State* lua){
	int len = luaL_checkinteger(lua, 1);
	if(len < 0){
		luaL_error(lua, "Vector length can't be negative");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = vector_init((uint)len);
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
	*vector = vector_init(len);
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
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	lua_pushinteger(lua, (*vector)->len);
	return 1;
}

static int l_vector_push(lua_State* lua){
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	double value = luaL_checknumber(lua, 2);
	vector_push(*vector, value);
	return 0;
}

static int l_vector_pop(lua_State* lua){
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	if(!(*vector)->len){
		luaL_error(lua, "Empty vector");
		return 0;
	}
	lua_pushnumber(lua, vector_pop(*vector));
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
		struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
		int index = luaL_checkinteger(lua, 2) - 1;
		if((uint)index > (*vector)->len || index < 0){
			luaL_error(lua, "Out of bound");
			return 0;
		}
		lua_pushnumber(lua, (*vector)->values[index]);
		return 1;
	}
	luaL_error(lua, "Invalid __index value");
	return 1;
}

static int l_vector_newindex(lua_State* lua){
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	int index = luaL_checkinteger(lua, 2) - 1;
	if((uint)index >= (*vector)->len || index < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	double value = luaL_checknumber(lua, 3);
	(*vector)->values[index] = value;
	return 0;
}

static int l_vector_gc(lua_State* lua){
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	vector_free(*vector);
	return 1;
}

static int l_vector_tostring(lua_State* lua){
	struct Vector** vector = luaL_checkudata(lua, 1, "CrunumVector");
	luaL_Buffer buffer;
	luaL_buffinit(lua, &buffer);
	luaL_addchar(&buffer, '{');
	for(uint i = 0; i < (*vector)->len; i++){
		char num[16];
		snprintf(num, sizeof(num), "%.2lf, ", (*vector)->values[i]);
		luaL_addstring(&buffer, num);
	}
	luaL_addchar(&buffer, '}');
	luaL_pushresult(&buffer);
	return 1;
}

static int l_vector_mul(lua_State* lua){
	struct Vector** vector1 = luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector** vector2 = luaL_testudata(lua, 2, "CrunumVector");
	if(vector2){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_mul(*vector1, *vector2);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Matrix** matrix = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = matrix_mul_vector(*matrix, *vector1);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_mul_scalar(*vector1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either Vector, Matrix or Scalar");
	return 0;
}

static int l_vector_add(lua_State* lua){
	struct Vector** vector1 = luaL_checkudata(lua, 1, "CrunumVector");
	struct Vector** vector2 = luaL_testudata(lua, 2, "CrunumVector");
	if(vector2){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_add(*vector1, *vector2);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = vector_add_scalar(*vector1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either Vector or Scalar");
	return 0;
}

const luaL_Reg vector_functions[] = {
	{"init", l_vector_init},
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
	{NULL, NULL}
};
