/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under the GPL-3.0 License. See LICENSE for details.
 */

#pragma message "Lua Matrix"

#include "lua_bind.h"

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

static int l_matrix_identity(lua_State* lua){
	int size = luaL_checkinteger(lua, 1);
	if(size < 0){
		luaL_error(lua, "Matrix dimension can't be negative");
		return 0;
	}
	struct Matrix** matrix = lua_newuserdata(lua, sizeof(struct Matrix*));
	*matrix = matrix_identity((uint)size);
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
	lua_pushnumber(lua, *matrix_get(matrix, (uint)row, (uint)col));
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
	float value = luaL_checknumber(lua, 4);
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

static int l_matrix_inverse(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(matrix->rows != matrix->cols){
		luaL_error(lua, "Matrix isn't a square");
		return 0;
	}
	uint invertible;
	struct Matrix* temp = matrix_inverse(matrix, &invertible);
	if(!invertible)
		luaL_error(lua, "Matrix can't be inversed");
	struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
	(*result) = temp;
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_push_row(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	matrix->cols = matrix->cols ? matrix->cols : vector->len;
	matrix->cols_cap = matrix->cols_cap ? matrix->cols_cap : vector->len;
	if(matrix->cols != vector->len){
		luaL_error(lua, "Matrix col size doesn't match vector length");
		return 0;
	}
	matrix_push_row(matrix, vector);
	return 0;
}

static int l_matrix_push_col(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Vector* vector = *(struct Vector**)luaL_checkudata(lua, 2, "CrunumVector");
	matrix->rows = matrix->rows ? matrix->rows : vector->len;
	matrix->rows_cap = matrix->rows_cap ? matrix->rows_cap : vector->len;
	if(matrix->rows != vector->len){
		luaL_error(lua, "Matrix row size doesn't match vector length");
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
	matrix_free(*(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix"));
	return 0;
}

static int l_matrix_tostring(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	luaL_Buffer result;
	luaL_buffinit(lua, &result);
	luaL_addchar(&result, '{');
	for(uint i = 0; i < matrix->rows; i++){
		luaL_addstring(&result, "\n  {");
		for(uint j = 0; j < matrix->cols; j++){
			char num[16];
			snprintf(num, sizeof(num), "%.2lf", *matrix_get(matrix, i, j));
			luaL_addstring(&result, num);
			if(j != matrix->cols - 1)
				luaL_addstring(&result, ", ");
		}
		luaL_addchar(&result, '}');
		if(i != matrix->rows - 1)
			luaL_addchar(&result, ',');
	}
	if(matrix->rows)
		luaL_addchar(&result, '\n');
	luaL_addchar(&result, '}');
	luaL_pushresult(&result);
	return 1;
}

static int l_matrix_add(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_add_scalar(matrix, luaL_checknumber(lua, 1));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
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

static int l_matrix_sub(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = scalar_sub_matrix(luaL_checknumber(lua, 1), matrix);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix2){
		if(matrix1->rows * matrix1->cols != 
				(*matrix2)->rows * (*matrix2)->cols){
			luaL_error(lua, "Matrix size doesn't match another matrix size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_sub(matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_sub_scalar(matrix1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix or scalar");
	return 0;
}

static int l_matrix_mul(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_mul_scalar(matrix, luaL_checknumber(lua, 1));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
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

static int l_matrix_div(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = scalar_div_matrix(luaL_checknumber(lua, 1), matrix);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	struct Matrix** matrix2 = luaL_testudata(lua, 2, "CrunumMatrix");
	if(matrix2){
		if(matrix1->rows * matrix1->cols != 
				(*matrix2)->rows * (*matrix2)->cols){
			luaL_error(lua, "Matrix size doesn't match another matrix size");
			return 0;
		}
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_div(matrix1, *matrix2);
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	if(lua_type(lua, 2) == LUA_TNUMBER){
		struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
		*result = matrix_div_scalar(matrix1, luaL_checknumber(lua, 2));
		luaL_getmetatable(lua, "CrunumMatrix");
		lua_setmetatable(lua, -2);
		return 1;
	}
	luaL_error(lua, "Right operand aren't either matrix or scalar");
	return 0;
}

static int l_matrix_pow(lua_State* lua){
	struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(matrix->rows != matrix->cols){
		luaL_error(lua, "Matrix isn't a square");
		return 0;
	}
	uint invertible;
	struct Matrix* pow = matrix_pow(matrix, luaL_checkinteger(lua, 2), &invertible);
	if(!invertible){
		luaL_error(lua, "Matrix can't be inversed");
		return 0;
	}
	struct Matrix** result = lua_newuserdata(lua, sizeof(struct Matrix*));
	*result = pow;
	luaL_getmetatable(lua, "CrunumMatrix");
	lua_setmetatable(lua, -2);
	return 1;
}

static int l_matrix_eq(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_eq_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_eq_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_eq(matrix1, matrix2));
	return 1;
}

static int l_matrix_neq(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_neq_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_neq_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_neq(matrix1, matrix2));
	return 1;
}

static int l_matrix_gt(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_gt_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_gt_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_gt(matrix1, matrix2));
	return 1;
}

static int l_matrix_ge(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_ge_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_ge_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_ge(matrix1, matrix2));
	return 1;
}

static int l_matrix_lt(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_lt_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_lt_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_lt(matrix1, matrix2));
	return 1;
}

static int l_matrix_le(lua_State* lua){
	if(lua_type(lua, 1) == LUA_TNUMBER){
		struct Matrix* matrix = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
		lua_pushboolean(lua, (int)!matrix_le_scalar(
					matrix, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix1 = *(struct Matrix**)luaL_checkudata(lua, 1, "CrunumMatrix");
	if(lua_type(lua, 2) == LUA_TNUMBER){
		lua_pushboolean(lua, (int)matrix_le_scalar(
					matrix1, luaL_checknumber(lua, 2)));
		return 1;
	}
	struct Matrix* matrix2 = *(struct Matrix**)luaL_checkudata(lua, 2, "CrunumMatrix");
	if(matrix1->rows != matrix2->rows ||
			matrix1->cols != matrix2->cols){
		luaL_error(lua, "Both matrix aren't the same size");
		return 0;
	}
	lua_pushboolean(lua, (int)matrix_le(matrix1, matrix2));
	return 1;
}

const luaL_Reg matrix_functions[] = {
	{"new", l_matrix_new},
	{"randinit", l_matrix_randinit},
	{"from", l_matrix_from},
	{"identity", l_matrix_identity},
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
	{"inverse", l_matrix_inverse},
	{"push_row", l_matrix_push_row},
	{"push_col", l_matrix_push_col},
	{"pop_row", l_matrix_pop_row},
	{"pop_col", l_matrix_pop_col},
	{"__gc", l_matrix_gc},
	{"__tostring", l_matrix_tostring},
	{"__add", l_matrix_add},
	{"__sub", l_matrix_sub},
	{"__mul", l_matrix_mul},
	{"__div", l_matrix_div},
	{"__pow", l_matrix_pow},
	{"__eq", l_matrix_eq},
	{"__neq", l_matrix_neq},
	{"__gt", l_matrix_gt},
	{"__ge", l_matrix_ge},
	{"__lt", l_matrix_lt},
	{"__le", l_matrix_le},
	{NULL, NULL}
};
