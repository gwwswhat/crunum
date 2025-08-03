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
#include "vector.h"

struct Vector* vector_new(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->cap = len;
#if defined(__ARM_NEON)
	posix_memalign((void**)&vector->values, 16, len * sizeof(float));
#else
	vector->values = malloc(len * sizeof(float));
#endif
	memset(vector->values, 0, len * sizeof(float));
	return vector;
}

struct Vector* vector_randinit(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->cap = len;
#if defined(__ARM_NEON)
	posix_memalign((void**)&vector->values, 16, len * sizeof(float));
#else
	vector->values = malloc(len * sizeof(float));
#endif
	for(uint i = 0; i < len; i++)
		vector->values[i] = (float)rand() / (float)RAND_MAX;
	return vector;
}

void vector_free(struct Vector* vector){
	free(vector->values);
	free(vector);
}

void vector_push(struct Vector* vector, float value){
	if(++vector->len > vector->cap){
		vector->cap = vector->cap ? vector->cap * 2 : 2;
		vector->values = realloc(vector->values, vector->cap * sizeof(float));
	}
	vector->values[vector->len - 1] = value;
}

struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if defined(__ARM_NEON)
	for(; i + 4 <= vector1->len; i += 4){
		float32x4_t vvec1 = vld1q_f32(vector1->values + i);
		float32x4_t vvec2 = vld1q_f32(vector2->values + i);
		float32x4_t vres = vmulq_f32(vvec1, vvec2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector1->len; i++)
		result->values[i] = vector1->values[i] * vector2->values[i];
	return result;
}

struct Vector* vector_mul_scalar(struct Vector* vector, float scalar){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if defined(__ARM_NEON)
	float32x4_t vscalar = {scalar, scalar, scalar, scalar};
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vmulq_f32(vvec, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = vector->values[i] * scalar;
	return result;
}

struct Vector* vector_mul_matrix(struct Vector* vector, struct Matrix* matrix){
	struct Vector* result = vector_new(vector->len);
	for(uint i = 0; i < vector->len; i++)
		for(uint j = 0; j < matrix->cols; j++){
			uint k = 0;
#if defined(__ARM_NEON)
			for(; k + 4 <= matrix->rows; k++){
				float32x4_t vvector = vld1q_f32(vector->values + i);
				float32x4_t vmatrix = vld1q_f32(matrix_get(matrix, j, k));
				float32x4_t vres = vmulq_f32(vvector, vmatrix);
				vst1q_f32(result->values + j, vres);
			}
#endif
			for(; k < matrix->rows; k++)
				result->values[j] += vector->values[i] * *matrix_get(matrix, j, k);
		}
	return result;
}

struct Vector* vector_add(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if defined(__ARM_NEON)
	for(; i + 4 <= vector1->len; i += 4){
		float32x4_t vvec1 = vld1q_f32(vector1->values + i);
		float32x4_t vvec2 = vld1q_f32(vector2->values + i);
		float32x4_t vres = vaddq_f32(vvec1, vvec2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector1->len; i++)
		result->values[i] = vector1->values[i] + vector2->values[i];
	return result;
}

struct Vector* vector_add_scalar(struct Vector* vector, float scalar){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if defined(__ARM_NEON)
	float32x4_t vscalar = {scalar, scalar, scalar, scalar};
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vaddq_f32(vvec, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = vector->values[i] + scalar;
	return result;
}
