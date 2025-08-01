/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#include "matrix.h"

struct Matrix* matrix_new(uint rows, uint cols){
	struct Matrix* matrix = malloc(sizeof(struct Matrix));
	matrix->rows = rows;
	matrix->cols = cols;
	matrix->rows_cap = rows;
	matrix->cols_cap = cols;
#if defined(__ARM_NEON)
	posix_memalign((void**)&matrix->values, 16, rows * cols * sizeof(float));
#else
	matrix->values = malloc(rows * cols * sizeof(float));
#endif
	memset(matrix->values, 0, rows * cols * sizeof(float));
	return matrix;
}

struct Matrix* matrix_randinit(uint rows, uint cols){
	srand(time(NULL));
	struct Matrix* matrix = malloc(sizeof(struct Matrix));
	matrix->rows = rows;
	matrix->cols = cols;
	matrix->rows_cap = rows;
	matrix->cols_cap = cols;
#if defined(__ARM_NEON)
	posix_memalign((void**)&matrix->values, 16, rows * cols * sizeof(float));
#else
	matrix->values = malloc(rows * cols * sizeof(float));
#endif
	memset(matrix->values, 0, rows * cols * sizeof(float));
	for(uint i = 0; i < rows * cols; i++)
		matrix->values[i] = (float)rand() / (float)RAND_MAX;
	return matrix;
}

void matrix_free(struct Matrix* matrix){
	free(matrix->values);
	free(matrix);
}

struct Vector* matrix_row(struct Matrix* matrix, uint row){
	struct Vector* vector = vector_new(matrix->cols);
	uint i = 0;
#if defined(__ARM_NEON)
	for(; i + 4 <= matrix->cols; i += 4){
		float32x4_t vtemp = {
			matrix_get(matrix, row, i),
			matrix_get(matrix, row, i + 1),
			matrix_get(matrix, row, i + 2),
			matrix_get(matrix, row, i + 3)
		};
		vst1q_f32(vector->values + i, vtemp);
	}
#endif
	for(; i < matrix->cols; i++)
		vector->values[i] = matrix_get(matrix, row, i);
	return vector;
}

struct Vector* matrix_col(struct Matrix* matrix, uint col){
	struct Vector* vector = vector_new(matrix->rows);
	uint i = 0;
#if defined(__ARM_NEON)
	for(; i + 4 <= matrix->rows; i += 4){
		float32x4_t vtemp = vld1q_f32(matrix->values + i);
		vst1q_f32(vector->values + i, vtemp);
	}
#endif
	for(; i < matrix->rows; i++)
		vector->values[i] = matrix_get(matrix, i, col);
	return vector;
}

void matrix_push_row(struct Matrix* matrix, struct Vector* vector){
	if(++matrix->rows > matrix->rows_cap){
		matrix->rows_cap = matrix->rows_cap ? matrix->rows_cap * 2 : 2;
		matrix->values = realloc(matrix->values, matrix->rows_cap * 
			matrix->cols_cap * sizeof(float));
	}
	for(uint i = 0; i < matrix->cols; i++)
		matrix_set(matrix, matrix->rows - 1, i, vector->values[i]);
}

void matrix_push_col(struct Matrix* matrix, struct Vector* vector){
	if(++matrix->cols > matrix->cols_cap)
		matrix->cols_cap = matrix->cols_cap ? matrix->cols_cap * 2 : 2;
	float* new_values = malloc(matrix->rows * matrix->cols_cap * 
			sizeof(float));
	for(uint i = 0; i < matrix->rows; i++){
		for(uint j = 0; j < matrix->cols - 1; j++)
			new_values[i * matrix->cols + j] = matrix_get(matrix, i, j);
		new_values[i * matrix->cols + matrix->cols - 1] = vector->values[i];
	}
	free(matrix->values);
	matrix->values = new_values;
}

struct Vector* matrix_pop_col(struct Matrix* matrix){
	float* new_values = malloc(matrix->rows * (matrix->cols - 1) * sizeof(float));
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
			float sum = 0;
			for(uint k = 0; k < matrix1->cols; k++)
				sum += matrix_get(matrix1, i, k) * matrix_get(matrix2, k, j);
			matrix_set(result, i, j, sum);
		}
	return result;
}

struct Matrix* matrix_mul_scalar(struct Matrix* matrix, float scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if defined(__ARM_NEON)
	float32x4_t vscalar = {scalar, scalar, scalar, scalar};
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vmulq_f32(vmatrix, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
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
	uint i = 0;
#if defined(__ARM_NEON)
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4){
		float32x4_t vmatrix1 = vld1q_f32(matrix1->values + i);
		float32x4_t vmatrix2 = vld1q_f32(matrix2->values + i);
		float32x4_t vres = vaddq_f32(vmatrix1, vmatrix2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		result->values[i] = matrix1->values[i] + matrix2->values[i];
	return result;
}

struct Matrix* matrix_add_scalar(struct Matrix* matrix, float scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if defined(__ARM_NEON)
	float32x4_t vscalar = {scalar, scalar, scalar, scalar};
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vmulq_f32(vmatrix, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
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
