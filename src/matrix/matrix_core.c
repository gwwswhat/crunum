/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under the GPL-3.0 License. See LICENSE for details.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "matrix.h"

struct Matrix* matrix_new(uint rows, uint cols){
	struct Matrix* matrix = malloc(sizeof(struct Matrix));
	matrix->rows = rows;
	matrix->cols = cols;
	matrix->rows_cap = rows;
	matrix->cols_cap = cols;
#if HAVE_NEON
	posix_memalign((void**)&matrix->values, NEON_ALIGNMENT,
			rows * cols * sizeof(float));
#else
	matrix->values = malloc(rows * cols * sizeof(float));
#endif
	memset(matrix->values, 0, rows * cols * sizeof(float));
	return matrix;
}

struct Matrix* matrix_init(uint rows, uint cols, float value){
	struct Matrix* matrix = matrix_new(rows, cols);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vvalue = vdupq_n_f32(value);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		vst1q_f32(matrix->values + i, vvalue);
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		matrix->values[i] = value;
	return matrix;
}

struct Matrix* matrix_randinit(uint rows, uint cols){
	struct Matrix* matrix = matrix_new(rows, cols);
	for(uint i = 0; i < rows * cols; i++)
		matrix->values[i] = (float)rand() / (float)RAND_MAX;
	return matrix;
}

struct Matrix* matrix_identity(uint size){
	struct Matrix* matrix = matrix_new(size, size);
	for(uint i = 0; i < matrix->rows; i++)
		matrix_set(matrix, i, i, 1);
	return matrix;
}

static struct Matrix* matrix_copy(struct Matrix* matrix){
	struct Matrix* copy = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		vst1q_f32(copy->values + i, vld1q_f32(matrix->values + i));
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		copy->values[i] = matrix->values[i];
	return copy;
}

void matrix_free(struct Matrix* matrix){
	free(matrix->values);
	free(matrix);
}

struct Vector* matrix_row(struct Matrix* matrix, uint row){
	struct Vector* vector = vector_new(matrix->cols);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->cols; i += 4){
		float32x4_t vtemp = {
			*matrix_get(matrix, row, i),
			*matrix_get(matrix, row, i + 1),
			*matrix_get(matrix, row, i + 2),
			*matrix_get(matrix, row, i + 3)
		};
		vst1q_f32(vector->values + i, vtemp);
	}
#endif
	for(; i < matrix->cols; i++)
		vector->values[i] = *matrix_get(matrix, row, i);
	return vector;
}

struct Vector* matrix_col(struct Matrix* matrix, uint col){
	struct Vector* vector = vector_new(matrix->rows);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->rows; i += 4){
		float32x4_t vtemp = vld1q_f32(matrix->values + i);
		vst1q_f32(vector->values + i, vtemp);
	}
#endif
	for(; i < matrix->rows; i++)
		vector->values[i] = *matrix_get(matrix, i, col);
	return vector;
}

void matrix_push_row(struct Matrix* matrix, struct Vector* vector){
	if(++matrix->rows > matrix->rows_cap){
		matrix->rows_cap = matrix->rows_cap ? matrix->rows_cap * 2 : 2;
		matrix->values = realloc(matrix->values, matrix->rows_cap * 
				matrix->cols_cap * sizeof(float));
	}
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->cols; i += 4){
		float32x4_t vvector = vld1q_f32(vector->values + i);
		vst1q_f32(matrix_get(matrix, matrix->rows - 1, i), vvector);
	}
#endif
	for(; i < matrix->cols; i++)
		matrix_set(matrix, matrix->rows - 1, i, vector->values[i]);
}

void matrix_push_col(struct Matrix* matrix, struct Vector* vector){
	if(++matrix->cols > matrix->cols_cap)
		matrix->cols_cap = matrix->cols_cap ? matrix->cols_cap * 2 : 2;
	float* new_values = malloc(matrix->rows * matrix->cols_cap * 
			sizeof(float));
	for(uint i = 0; i < matrix->rows; i++){
		uint j = 0;
#if HAVE_NEON
		for(; j + 4 <= matrix->cols - 1; j += 4){
			float32x4_t vvector = vld1q_f32(vector->values + j);
			vst1q_f32(matrix_get(matrix, i, j), vvector);
		}
#endif
		for(; j < matrix->cols - 1; j++)
			new_values[i * matrix->cols + j] = *matrix_get(matrix, i, j);
		new_values[i * matrix->cols + matrix->cols - 1] = vector->values[i];
	}
	free(matrix->values);
	matrix->values = new_values;
}

struct Vector* matrix_pop_col(struct Matrix* matrix){
	float* new_values = malloc(matrix->rows * (matrix->cols - 1) * sizeof(float));
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = 0; j < matrix->cols - 1; j++)
			new_values[i * (matrix->cols - 1) + j] = *matrix_get(matrix, i, j);
	free(matrix->values);
	matrix->values = new_values;
	return matrix_col(matrix, --matrix->cols);
}

struct Matrix* matrix_transpose(struct Matrix* matrix){
	struct Matrix* new_matrix = matrix_new(matrix->cols, matrix->rows);
	for(uint i = 0; i < matrix->rows; i++)
		for(uint j = 0; j < matrix->cols; j++)
			matrix_set(new_matrix, j, i, *matrix_get(matrix, i, j));
	return new_matrix;
}

static void matrix_swap_row(struct Matrix* matrix, uint row1, uint row2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->cols; i += 4){
		float32x4_t vtemp = vld1q_f32(matrix_get(matrix, row1, i));
		vst1q_f32(matrix_get(matrix, row1, i),
				vld1q_f32(matrix_get(matrix, row2, i)));
		vst1q_f32(matrix_get(matrix, row2, i), vtemp);
	}
#endif
	for(; i < matrix->cols; i++){
		float temp = *matrix_get(matrix, row1, i);
		matrix_set(matrix, row1, i, *matrix_get(matrix, row2, i));
		matrix_set(matrix, row2, i, temp);
	}
}

static void normalize_pivot(struct Matrix* matrix, uint row, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->cols; i += 4){
		float32x4_t vrow = vld1q_f32(matrix_get(matrix, row, i));
		float32x4_t vres = vdivq_f32(vrow, vscalar);
		vst1q_f32(matrix_get(matrix, row, i), vres);
	}
#endif
	for(; i < matrix->cols; i++)
		*matrix_get(matrix, row, i) /= scalar;
}

static void eliminate_not_pivot(struct Matrix* matrix, struct Matrix* identity, uint row){
	for(uint i = 0; i < matrix->rows; i++){
		if(i == row)
			continue;
		float scalar = *matrix_get(matrix, i, row);
		uint j = 0;
#if HAVE_NEON
		for(; j + 4 <= matrix->cols; j += 4){
			float32x4_t vscalar = vdupq_n_f32(scalar);
			float32x4_t vmul = vmulq_f32(vscalar, 
					vld1q_f32(matrix_get(matrix, row, j)));
			float32x4_t vidmul = vmulq_f32(vscalar, 
					vld1q_f32(matrix_get(identity, row, j)));
			float32x4_t vsub = vsubq_f32(vld1q_f32(matrix_get(matrix, i, j)), 
					vmul);
			float32x4_t vidsub = vsubq_f32(vld1q_f32(matrix_get(identity, i, j)),
					vidmul);
			float32x4_t vres = vsubq_f32(vld1q_f32(matrix_get(matrix, i, j)),
					vsub);
			float32x4_t vidres = vsubq_f32(vld1q_f32(matrix_get(identity, i, j)),
					vidsub);
			vst1q_f32(matrix_get(matrix, i, j), vres);
			vst1q_f32(matrix_get(identity, i, j), vidres);
		}
#endif
		for(; j < matrix->cols; j++){
			*matrix_get(matrix, i, j) -= scalar * *matrix_get(matrix, row, j);
			*matrix_get(identity, i, j) -= scalar * *matrix_get(identity, row, j);
		}
	}
}

struct Matrix* matrix_inverse(struct Matrix* matrix, uint* invertible){
	*invertible = 1;
	struct Matrix* result = matrix_identity(matrix->rows);
	struct Matrix* copy_matrix = matrix_copy(matrix);
	for(uint i = 0; i < copy_matrix->rows; i++){
		if(fabsf(*matrix_get(copy_matrix, i, i)) < NEAR_ZERO){
			*invertible = 0;
			for(uint j = i + 1; j < copy_matrix->rows; j++)
				if(fabsf(*matrix_get(copy_matrix, j, i)) > NEAR_ZERO){
					*invertible = 1;
					matrix_swap_row(copy_matrix, i, j);
					matrix_swap_row(result, i, j);
					break;
				}
			if(!*invertible){
				matrix_free(result);
				matrix_free(copy_matrix);
				return NULL;
			}
		}
		float pivot_value = *matrix_get(copy_matrix, i, i);
		normalize_pivot(copy_matrix, i, pivot_value);
		normalize_pivot(result, i, pivot_value);
		eliminate_not_pivot(copy_matrix, result, i);
	}
	matrix_free(copy_matrix);
	return result;
}
