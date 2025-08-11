/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under the GPL-3.0 License. See LICENSE for details.
 */

#include "config.h"
#include "matrix.h"
#include "vector.h"

struct Vector* vector_add(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if HAVE_NEON
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
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
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

struct Vector* vector_sub(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4){
		float32x4_t vvec1 = vld1q_f32(vector1->values + i);
		float32x4_t vvec2 = vld1q_f32(vector2->values + i);
		float32x4_t vres = vsubq_f32(vvec1, vvec2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector1->len; i++)
		result->values[i] = vector1->values[i] - vector2->values[i];
	return result;
}

struct Vector* vector_sub_scalar(struct Vector* vector, float scalar){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vsubq_f32(vvec, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = vector->values[i] - scalar;
	return result;
}

struct Vector* scalar_sub_vector(float scalar, struct Vector* vector){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vsubq_f32(vscalar, vvec);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = scalar - vector->values[i];
	return result;
}

struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if HAVE_NEON
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
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
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
#if HAVE_NEON
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

struct Vector* vector_div(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_new(vector1->len);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4){
		float32x4_t vvec1 = vld1q_f32(vector1->values + i);
		float32x4_t vvec2 = vld1q_f32(vector2->values + i);
		float32x4_t vres = vdivq_f32(vvec1, vvec2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector1->len; i++)
		result->values[i] = vector1->values[i] / vector2->values[i];
	return result;
}

struct Vector* vector_div_scalar(struct Vector* vector, float scalar){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vdivq_f32(vvec, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = vector->values[i] / scalar;
	return result;
}

struct Vector* scalar_div_vector(float scalar, struct Vector* vector){
	struct Vector* result = vector_new(vector->len);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4){
		float32x4_t vvec = vld1q_f32(vector->values + i);
		float32x4_t vres = vdivq_f32(vscalar, vvec);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < vector->len; i++)
		result->values[i] = scalar / vector->values[i];
	return result;
}

uint vector_eq(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_neq(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] != vector2->values[i])
			return 0;
	return 1;
}

uint vector_neq(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_eq(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] == vector2->values[i])
			return 0;
	return 1;
}

uint vector_gt(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_le(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] <= vector2->values[i])
			return 0;
	return 1;
}

uint vector_ge(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_lt(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] < vector2->values[i])
			return 0;
	return 1;
}

uint vector_lt(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_ge(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] >= vector2->values[i])
			return 0;
	return 1;
}

uint vector_le(struct Vector* vector1, struct Vector* vector2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= vector1->len; i += 4)
		if(is_lanes_gt(vld1q_f32(vector1->values + i),
					vld1q_f32(vector2->values + i)))
			return 0;
#endif
	for(; i < vector1->len; i++)
		if(vector1->values[i] > vector2->values[i])
			return 0;
	return 1;
}

uint vector_eq_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_neq(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] != scalar)
			return 0;
	return 1;
}

uint vector_neq_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_eq(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] == scalar)
			return 0;
	return 1;
}

uint vector_gt_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_le(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] <= scalar)
			return 0;
	return 1;
}

uint vector_ge_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_lt(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] < scalar)
			return 0;
	return 1;
}

uint vector_lt_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_ge(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] >= scalar)
			return 0;
	return 1;
}

uint vector_le_scalar(struct Vector* vector, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= vector->len; i += 4)
		if(is_lanes_gt(vld1q_f32(vector->values + i), vscalar))
			return 0;
#endif
	for(; i < vector->len; i++)
		if(vector->values[i] > scalar)
			return 0;
	return 1;
}
