#ifndef CRUNUM_MATRIX_H
#define CRUNUM_MATRIX_H

#include "vector.h"
#include "common.h"

extern const luaL_Reg matrix_functions[];
extern const luaL_Reg matrix_methods[];

struct Matrix {
	float* values;
	uint rows;
	uint cols;
	uint rows_cap;
	uint cols_cap;
};

struct Matrix* matrix_new(uint rows, uint cols);
struct Matrix* matrix_randinit(uint rows, uint cols);
void matrix_free(struct Matrix* matrix);
static inline float* matrix_get(struct Matrix* matrix, uint i, uint j){
	return &matrix->values[i * matrix->cols + j];
}

static inline void matrix_set(struct Matrix* matrix, uint i, uint j, float value){
	matrix->values[i * matrix->cols + j] = value;
}

struct Vector* matrix_row(struct Matrix* matrix, uint row);
struct Vector* matrix_col(struct Matrix* matrix, uint col);
void matrix_push_row(struct Matrix* matrix, struct Vector* vector);
void matrix_push_col(struct Matrix* matrix, struct Vector* vector);
static inline struct Vector* matrix_pop_row(struct Matrix* matrix){
	return matrix_row(matrix, --matrix->rows);
}

struct Vector* matrix_pop_col(struct Matrix* matrix);
struct Matrix* matrix_mul(struct Matrix* matrix1, struct Matrix* matrix2);
struct Matrix* matrix_mul_scalar(struct Matrix* matrix, float scalar);
struct Vector* matrix_mul_vector(struct Matrix* matrix, struct Vector* vector);
struct Matrix* matrix_add(struct Matrix* matrix1, struct Matrix* matrix2);
struct Matrix* matrix_add_scalar(struct Matrix* matrix, float scalar);
struct Matrix* matrix_transpose(struct Matrix* matrix);
static inline void matrix_reshape(struct Matrix* matrix, 
		uint new_rows, uint new_cols){
	matrix->rows = new_rows;
	matrix->cols = new_cols;
}

#endif
