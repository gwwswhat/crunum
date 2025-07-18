#include <stdlib.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

#include "matrix.h"

struct Matrix* matrix_init(uint rows, uint cols){
	struct Matrix* matrix = malloc(sizeof(struct Matrix));
	matrix->rows = rows;
	matrix->cols = cols;
	matrix->values = calloc(1, rows * cols * sizeof(double));
	return matrix;
}

struct Matrix* matrix_randinit(uint rows, uint cols){
	struct Matrix* matrix = malloc(sizeof(struct Matrix));
	matrix->rows = rows;
	matrix->cols = cols;
	matrix->values = malloc(rows * cols * sizeof(double));
	for(uint i = 0; i < rows * cols; i++)
		matrix->values[i] = (double)rand() / (double)RAND_MAX;
	return matrix;
}

void matrix_free(struct Matrix* matrix){
	free(matrix->values);
	free(matrix);
}

struct Matrix* matrix_mul(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_init(matrix1->rows, matrix2->cols);
	for(uint i = 0; i < matrix1->rows; i++)
		for(uint j = 0; j < matrix2->cols; j++){
			double sum = 0;
			for(uint k = 0; k < matrix1->cols; k++)
				sum += matrix_get(matrix1, i, k) * matrix_get(matrix2, k, j);
			matrix_set(result, i, j, sum);
		}
	return result;
}

struct Matrix* matrix_mul_scalar(struct Matrix* matrix, double scalar){
	struct Matrix* result = matrix_init(matrix->rows, matrix->cols);
	for(uint i = 0; i < result->rows * result->cols; i++)
		result->values[i] = matrix->values[i] * scalar;
	return result;
}

struct Vector* matrix_mul_vector(struct Matrix* matrix, struct Vector* vector){
	struct Vector* result = vector_init(vector->len);
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = 0; j < vector->len; j++)
			for(uint k = 0; k < matrix->cols; k++)
				result->values[j] += matrix_get(matrix, i, k) * vector->values[j];
	return result;
}

struct Matrix* matrix_sum(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_init(matrix1->rows, matrix1->cols);
	for(uint i = 0; i < matrix1->rows * matrix1->cols; i++)
		result->values[i] = matrix1->values[i] + matrix2->values[i];
	return result;
}

struct Matrix* matrix_transpose(struct Matrix* matrix){
	struct Matrix* new_matrix = matrix_init(matrix->cols, matrix->rows);
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = i + 1; j < matrix->cols; j++)
			matrix_set(new_matrix, j, i, matrix_get(matrix, i, j));
	return new_matrix;
}

static int l_matrix_init(lua_State* lua){
	int rows = luaL_checkinteger(lua, 1);
	int cols = luaL_checkinteger(lua, 2);
	if(rows < 1 || cols < 1){
		luaL_error("Matrix dimension must be positive integers");
		return 0;
	}
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_init((uint)rows, (uint)cols);
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_randinit(lua_State* lua){
	int rows = luaL_checkinteger(lua, 1);
	int cols = luaL_checkinteger(lua, 2);
	if(rows < 1 || cols < 1){
		luaL_error("Matrix dimension must be positive integers");
		return 0;
	}
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_randinit((uint)rows, (uint)cols);
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_from(lua_State* lua){
	luaL_checktype(lua, 1, LUA_TTABLE);
	uint rows = luaL_rawlen(lua, 1);
	lua_rawgeti(lua, 1, 1);
	luaL_checktype(lua, -1, LUA_TTABLE);
	uint cols = luaL_rawlen(lua, -1);
	lua_pop(lua, 1);
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_init(rows, cols);
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	for(uint i = 0; i < rows; i++){
		lua_rawgeti(lua, 1, i + 1);
		luaL_checktype(lua, -1, LUA_TTABLE);
		if(luaL_rawlen(lua, -1) != cols){
			luaL_error(lua, "Inconsistent size");
			return 0;
		}
		for(uint j = 0; j < cols; j++){
			lua_rawgeti(lua, -1, j + 1);
			matrix_set(*matrix, i, j, luaL_checkinteger(lua, -1));
			lua_pop(lua, 1);
		}
		lua_pop(lua, 1);
	}	
	return 1;
}

static int l_matrix_rows(lua_State* lua){
	struct Matrix** matrix = luaL_checkudata(lua, 1, "CrunumMatrix");
	lua_pushinteger(lua, (*matrix)->rows);
	return 1;
}

static int l_matrix_cols(lua_State* lua){
	struct Matrix** matrix = luaL_checkudata(lua, 1, "CrunumMatrix");
	lua_pushinteger(lua, (*matrix)->cols);
	return 1;
}

static int l_matrix_index(lua_State* lua){
	int type = lua_type(lua, 2);
	if(type == LUA_TSTRING){
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_pushvalue(lua, 2);
		lua_rawget(lua, -2);
		return 1;
	}
	else if(type == LUA_TNUMBER){
		struct Matrix** matrix = luaL_checkudata(lua, 1, "CrunumMatrix");
		int row = luaL_checkinteger(lua, 2) - 1);
		if(row > (*matrix)->rows || row < 0){
			luaL_error(lua, "Out of bound");
			return 0;
		}
		struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
		*vector = matrix_row(*matrix, (uint)row);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Invalid __index value");
	return 0;
}

static int l_matrix_gc(lua_State* lua){
	struct Matrix** matrix = luaL_checkudata(lua, 1, "CrunumMatrix");
	matrix_free(*matrix);
	return 0;
}

static int l_matrix_tostring(lua_State* lua){
	struct Matrix** matrix = luaL_checkudata(lua, 1, "CrunumMatrix");
	luaL_Buffer buffer;
	luaL_buffinit(lua, &buffer);
	luaL_addstring(&buffer, "{\n");
	for(uint i = 0; i < (*matrix)->rows; i++){
		luaL_addstring(&buffer, "  {");
		for(uint j = 0; j < (*matrix)->cols; j++){
			char num[16];
			snprintf(num, sizeof(num), "%.2lf, ", matrix_get(i, j));
			luaL_addstring(&buffer, num);
		}
	}
	luaL_pushresult(&buffer);
	return 1;
}

static int l_matrix_mul(lua_State* lua){
	struct Matrix** matrix1 = luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(*matrix2){
		if((*matrix1)->rows != (*matrix2)->cols){
			luaL_error(lua, "Matrix row size doesn't match another matrix col size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_mul(*matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Vector** vector = luaL_testudata(lua, 2, "CrunumVector");
	if(*vector){
		if((*matrix1)->cols != (*vector)->len){
			luaL_error(lua, "Matrix col size doesn't match vector length");
			return 0;
		}
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = matrix_mul_vector(*matrix1, *vector);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_mul_scalar(*matrix1, lua_tonumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix, vector or scalar");
	return 0;
}

static int l_matrix_mul(lua_State* lua){
	struct Matrix** matrix1 = luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(*matrix2){
		if((*matrix1)->rows * (*matrix1)->cols != 
				(*matrix2)->rows * (*matrix2)->cols){
			luaL_error(lua, "Matrix size doesn't match another matrix size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_sum(*matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_sum_scalar(*matrix1, lua_tonumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix or scalar");
	return 0;
}

static const luaL_Reg matrix_functions[] = {
	{"init", l_matrix_init},
	{"randinit", l_matrix_randinit},
	{"from", l_matrix_from},
	{NULL, NULL}
};

static const luaL_Reg matrix_methods[] = {
	{"rows", l_matrix_rows},
	{"cols", l_matrix_cols},
	{"__index", l_matrix_index},
	{"__gc", l_matrix_gc},
	{"__tostring", l_matrix_tostring},
	{"__mul", l_matrix_mul},
	{"__add", l_matrix_sum},
	{NULL, NULL}
};
