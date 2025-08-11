/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "vector.h"

struct Vector* vector_new(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->cap = len;
#if HAVE_NEON
	posix_memalign((void**)&vector->values, NEON_ALIGNMENT,
			len * sizeof(float));
#else
	vector->values = malloc(len * sizeof(float));
#endif
	memset(vector->values, 0, len * sizeof(float));
	return vector;
}

struct Vector* vector_init(uint len, float value){
	struct Vector* vector = vector_new(len);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(value);
	for(; i + 4 <= vector->len; i += 4)
		vst1q_f32(vector->values + i, vscalar);
#endif
	for(; i < vector->len; i++)
		vector->values[i] = value;
	return vector;
}

struct Vector* vector_randinit(uint len){
	struct Vector* vector = vector_new(len);
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
