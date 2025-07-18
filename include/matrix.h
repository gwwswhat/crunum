#ifndef CRUNUM_MATRIX_H
#define CRUNUM_MATRIX_H

#include "vector.h"
#include "common.h"

struct Matrix {
	double* values;
	uint rows;
	uint cols;
};

struct Matrix* matrix_init(uint rows, uint cols);
struct Matrix* matrix_randinit(uint rows, uint cols);
void matrix_free(struct Matrix* matrix);
static inline double matrix_get(struct Matrix* matrix, uint row, uint col){
	return matrix->values[row * matrix->cols + col];
}

static inline void matrix_set(struct Matrix* matrix, uint row, uint col, double value){
	matrix->values[row * matrix->cols + col] = value;
}

struct Matrix* matrix_mul(struct Matrix* matrix1, struct Matrix* matrix2);
struct Matrix* matrix_mul_scalar(struct Matrix* matrix, double scalar);
struct Vector* matrix_mul_vector(struct Matrix* matrix, struct Vector* vector);
struct Matrix* matrix_sum(struct Matrix* matrix1, struct Matrix* matrix2);
struct Matrix* matrix_sum_scalar(struct Matrix* matrix, double scalar);
struct Matrix* matrix_transpose(struct Matrix* matrix);
static inline void matrix_reshape(struct Matrix* matrix, 
		uint new_rows, uint new_cols){
	matrix->rows = new_rows;
	matrix->cols = new_cols;
}

#endif
