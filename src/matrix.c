/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#include <stdlib.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

#include "matrix.h"

struct Matrix* matrix_new(uint rows, uint cols){
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

struct Vector* matrix_row(struct Matrix* matrix, uint row){
	struct Vector* vector = vector_new(matrix->cols);
	for(uint i = 0; i < matrix->cols; i++)
		vector->values[i] = matrix_get(matrix, row, i);
	return vector;
}

struct Vector* matrix_col(struct Matrix* matrix, uint col){
	struct Vector* vector = vector_new(matrix->rows);
	for(uint i = 0; i < matrix->rows; i++)
		vector->values[i] = matrix_get(matrix, i, col);
	return vector;
}

void matrix_push_row(struct Matrix* matrix, struct Vector* vector){
	matrix->rows++;
	matrix->values = realloc(matrix->values, matrix->rows * matrix->cols *
			sizeof(double));
	for(uint i = 0; i < matrix->cols; i++)
		matrix_set(matrix, matrix->rows - 1, i, vector->values[i]);
}

void matrix_push_col(struct Matrix* matrix, struct Vector* vector){
	matrix->cols++;
	double* new_values = malloc(matrix->rows * matrix->cols * sizeof(double));
	for(uint i = 0; i < matrix->rows; i++){
		for(uint j = 0; j < matrix->cols - 1; j++)
			new_values[i * matrix->cols + j] = matrix_get(matrix, i, j);
		new_values[i * matrix->cols + matrix->cols - 1] = vector->values[i];
	}
	free(matrix->values);
	matrix->values = new_values;
}

struct Vector* matrix_pop_col(struct Matrix* matrix){
	double* new_values = malloc(matrix->rows * (matrix->cols - 1) * sizeof(double));
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = 0; j < matrix->cols - 1; j++)
			new_values[i * (matrix->cols - 1) + j] = matrix_get(matrix, i, j);
	free(matrix->values);
	matrix->values = new_values;
	return matrix_col(matrix, --matrix->cols);
}

struct Matrix* matrix_mul(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix2->cols);
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
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	for(uint i = 0; i < result->rows * result->cols; i++)
		result->values[i] = matrix->values[i] * scalar;
	return result;
}

struct Vector* matrix_mul_vector(struct Matrix* matrix, struct Vector* vector){
	struct Vector* result = vector_new(vector->len);
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = 0; j < vector->len; j++)
			for(uint k = 0; k < matrix->cols; k++)
				result->values[j] += matrix_get(matrix, i, k) * vector->values[j];
	return result;
}

struct Matrix* matrix_add(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix1->cols);
	for(uint i = 0; i < matrix1->rows * matrix1->cols; i++)
		result->values[i] = matrix1->values[i] + matrix2->values[i];
	return result;
}

struct Matrix* matrix_add_scalar(struct Matrix* matrix, double scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	for(uint i = 0; i < matrix->rows * matrix->cols; i++)
		result->values[i] = matrix->values[i] + scalar;
	return result;
}

struct Matrix* matrix_transpose(struct Matrix* matrix){
	struct Matrix* new_matrix = matrix_new(matrix->cols, matrix->rows);
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = i + 1; j < matrix->cols; j++)
			matrix_set(new_matrix, j, i, matrix_get(matrix, i, j));
	return new_matrix;
}

static int l_matrix_new(lua_State* lua){
	int rows = luaL_checkinteger(lua, 1);
	int cols = luaL_checkinteger(lua, 2);
	if(rows < 0 || cols < 0){
		luaL_error(lua, "Matrix dimension can't be negative");
		return 0;
	}
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_new((uint)rows, (uint)cols);
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_randinit(lua_State* lua){
	int rows = luaL_checkinteger(lua, 1);
	int cols = luaL_checkinteger(lua, 2);
	if(rows < 0 || cols < 0){
		luaL_error(lua, "Matrix dimension can't be negative");
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
	uint rows = lua_rawlen(lua, 1);
	lua_rawgeti(lua, 1, 1);
	luaL_checktype(lua, -1, LUA_TTABLE);
	uint cols = lua_rawlen(lua, -1);
	lua_pop(lua, 1);
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_new(rows, cols);
	for(uint i = 0; i < rows; i++){
		lua_rawgeti(lua, 1, i + 1);
		luaL_checktype(lua, -1, LUA_TTABLE);
		if(lua_rawlen(lua, -1) != cols){
			luaL_error(lua, "Inconsistent size");
			return 0;
		}
		for(uint j = 0; j < cols; j++){
			lua_rawgeti(lua, -1, j + 1);
			matrix_set(*matrix, i, j, luaL_checknumber(lua, -1));
			lua_pop(lua, 1);
		}
		lua_pop(lua, 1);
	}	
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_get(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	int row = luaL_checkinteger(lua, 2) - 1;
	int col = luaL_checkinteger(lua, 3) - 1;
	if((uint)row >= matrix->rows || (uint)col >= matrix->rows || 
			row < 0 || col < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	lua_pushnumber(lua, matrix_get(matrix, (uint)row, (uint)col));
	return 1;
}

static int l_matrix_set(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	int row = luaL_checkinteger(lua, 2) - 1;
	int col = luaL_checkinteger(lua, 3) - 1;
	if((uint)row >= matrix->rows || (uint)col >= matrix->rows || 
			row < 0 || col < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	double value = luaL_checknumber(lua, 4);
	matrix_set(matrix, (uint)row, (uint)col, value);
	return 0;
}

static int l_matrix_row(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	int row = luaL_checkinteger(lua, 2) - 1;
	if((uint)row >= matrix->rows || row < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = matrix_row(matrix, (uint)row);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_col(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	int col = luaL_checkinteger(lua, 2) - 1;
	if((uint)col >= matrix->cols || col < 0){
		luaL_error(lua, "Out of bound");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = matrix_col(matrix, (uint)col);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_rows(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	lua_pushinteger(lua, matrix->rows);
	return 1;
}

static int l_matrix_cols(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	lua_pushinteger(lua, matrix->cols);
	return 1;
}

static int l_matrix_transpose(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
	*result = matrix_transpose(matrix);
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_reshape(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	int new_rows = luaL_checkinteger(lua, 2);
	int new_cols = luaL_checkinteger(lua, 3);
	if(new_rows < 0 || new_cols < 0){
		luaL_error(lua, "Matrix dimension can't be negative");
		return 0;
	}
	if((uint)new_rows * (uint)new_cols != matrix->rows * matrix->cols){
		luaL_error(lua, "New size is different with old one");
		return 0;
	}
	matrix_reshape(matrix, (uint)new_rows, (uint)new_cols);
	return 0;
}

static int l_matrix_push_row(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	matrix->cols = matrix->cols ? matrix->cols : vector->len;
	if(vector->len != matrix->cols){
		luaL_error(lua, "Vector length doesn't match matrix col size");
		return 0;
	}
	matrix_push_row(matrix, vector);
	return 0;
}

static int l_matrix_push_col(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	matrix->rows = matrix->rows ? matrix->rows : vector->len;
	if(vector->len != matrix->rows){
		luaL_error(lua, "Vector length doesn't match matrix row size");
		return 0;
	}
	matrix_push_col(matrix, vector);
	return 0;
}

static int l_matrix_pop_row(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(!matrix->rows){
		luaL_error(lua, "Empty matrix");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = matrix_pop_row(matrix);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_pop_col(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(!matrix->cols){
		luaL_error(lua, "Empty matrix");
		return 0;
	}
	struct Vector** vector = lua_newuserdata(lua, sizeof(struct Vector*));
	*vector = matrix_pop_col(matrix);
	luaL_getmetatable(lua, "CrunumVector");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_gc(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	matrix_free(matrix);
	return 0;
}

static int l_matrix_tostring(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	luaL_Buffer buffer;
	luaL_buffinit(lua, &buffer);
	luaL_addstring(&buffer, "{\n");
	for(uint i = 0; i < matrix->rows; i++){
		luaL_addstring(&buffer, "  {");
		for(uint j = 0; j < matrix->cols; j++){
			char num[16];
			snprintf(num, sizeof(num), "%.2lf, ", matrix_get(matrix, i, j));
			luaL_addstring(&buffer, num);
		}
		luaL_addstring(&buffer, "},\n");
	}
	luaL_addchar(&buffer, '}');
	luaL_pushresult(&buffer);
	return 1;
}

static int l_matrix_mul(lua_State* lua){
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix2){
		if(matrix1->rows != (*matrix2)->cols){
			luaL_error(lua, "Matrix row size doesn't match another matrix col size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_mul(matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Vector** vector = luaL_testudata(lua, 2, "CrunumVector");
	if(vector){
		if(matrix1->cols != (*vector)->len){
			luaL_error(lua, "Matrix col size doesn't match vector length");
			return 0;
		}
		struct Vector** result = lua_newuserdata(lua, sizeof(struct Vector*));
		*result = matrix_mul_vector(matrix1, *vector);
		luaL_getmetatable(lua, "CrunumVector");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_mul_scalar(matrix1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix, vector or scalar");
	return 0;
}

static int l_matrix_add(lua_State* lua){
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix2){
		if(matrix1->rows * matrix1->cols != 
				(*matrix2)->rows * (*matrix2)->cols){
			luaL_error(lua, "Matrix size doesn't match another matrix size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_add(matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_add_scalar(matrix1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix or scalar");
	return 0;
}

const luaL_Reg matrix_functions[] = {
	{"new", l_matrix_new},
	{"randinit", l_matrix_randinit},
	{"from", l_matrix_from},
	{NULL, NULL}
};

const luaL_Reg matrix_methods[] = {
	{"get", l_matrix_get},
	{"set", l_matrix_set},
	{"row", l_matrix_row},
	{"col", l_matrix_col},
	{"rows", l_matrix_rows},
	{"cols", l_matrix_cols},
	{"transpose", l_matrix_transpose},
	{"reshape", l_matrix_reshape},
	{"push_row", l_matrix_push_row},
	{"push_col", l_matrix_push_col},
	{"pop_row", l_matrix_pop_row},
	{"pop_col", l_matrix_pop_col},
	{"__gc", l_matrix_gc},
	{"__tostring", l_matrix_tostring},
	{"__mul", l_matrix_mul},
	{"__add", l_matrix_add},
	{NULL, NULL}
};
