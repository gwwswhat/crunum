/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under the GPL-3.0 License. See LICENSE for details.
 */

#include "config.h"

#include <stddef.h>

#include "matrix.h"

struct Matrix* matrix_add(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix1->cols);
	uint i = 0;
#if HAVE_NEON
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
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
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

struct Matrix* matrix_sub(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix1->cols);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4){
		float32x4_t vmatrix1 = vld1q_f32(matrix1->values + i);
		float32x4_t vmatrix2 = vld1q_f32(matrix2->values + i);
		float32x4_t vres = vsubq_f32(vmatrix1, vmatrix2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		result->values[i] = matrix1->values[i] - matrix2->values[i];
	return result;
}

struct Matrix* matrix_sub_scalar(struct Matrix* matrix, float scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vsubq_f32(vmatrix, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		result->values[i] = matrix->values[i] - scalar;
	return result;
}

struct Matrix* scalar_sub_matrix(float scalar, struct Matrix* matrix){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vsubq_f32(vscalar, vmatrix);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		result->values[i] = scalar - matrix->values[i];
	return result;
}

struct Matrix* matrix_mul(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix2->cols);
	for(uint i = 0; i < matrix1->rows; i++)
		for(uint j = 0; j < matrix2->cols; j++){
			uint k = 0;
#if HAVE_NEON
			for(; k + 4 <= matrix1->cols; k += 4){
				float32x4_t vmatrix1 = vld1q_f32(matrix_get(matrix1, i, k));
				float32x4_t vmatrix2 = vld1q_f32(matrix_get(matrix2, k, j));
				float32x4_t vres = vmulq_f32(vmatrix1, vmatrix2);
				matrix_set(result, i, j, *matrix_get(result, i, j) + p_vaddvq_f32(vres));
			}
#endif
			for(; k < matrix1->cols; k++)
				matrix_set(result, i, j, *matrix_get(result, i, j) +
						*matrix_get(matrix1, i, k) * *matrix_get(matrix2, k, j));
		}
	return result;
}

struct Matrix* matrix_mul_scalar(struct Matrix* matrix, float scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
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
		for(uint j = 0; j < vector->len; j++){
			uint k = 0;
#if HAVE_NEON
			for(; k + 4 <= matrix->cols; k += 4){
				float32x4_t vmatrix = vld1q_f32(matrix_get(matrix, i, k));
				float32x4_t vvector = vld1q_f32(vector->values + j);
				float32x4_t vres = vmulq_f32(vmatrix, vvector);
				vst1q_f32(result->values + j, vres);
			}
#endif
			for(; k < matrix->cols; k++)
				result->values[j] += *matrix_get(matrix, i, k) * vector->values[j];
		}
	return result;
}

struct Matrix* matrix_div(struct Matrix* matrix1, struct Matrix* matrix2){
	struct Matrix* result = matrix_new(matrix1->rows, matrix1->cols);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4){
		float32x4_t vmatrix1 = vld1q_f32(matrix1->values + i);
		float32x4_t vmatrix2 = vld1q_f32(matrix2->values + i);
		float32x4_t vres = vdivq_f32(vmatrix1, vmatrix2);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		result->values[i] = matrix1->values[i] / matrix2->values[i];
	return result;
}

struct Matrix* matrix_div_scalar(struct Matrix* matrix, float scalar){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vdivq_f32(vmatrix, vscalar);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		result->values[i] = matrix->values[i] / scalar;
	return result;
}

struct Matrix* scalar_div_matrix(float scalar, struct Matrix* matrix){
	struct Matrix* result = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4){
		float32x4_t vmatrix = vld1q_f32(matrix->values + i);
		float32x4_t vres = vdivq_f32(vscalar, vmatrix);
		vst1q_f32(result->values + i, vres);
	}
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		result->values[i] = scalar / matrix->values[i];
	return result;
}

struct Matrix* matrix_pow(struct Matrix* matrix, int exp, uint* invertible){
	struct Matrix* result = matrix_identity(matrix->rows);
	if(!exp)
		return result;
	struct Matrix* base = matrix_new(matrix->rows, matrix->cols);
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		vst1q_f32(base->values + i, vld1q_f32(matrix->values + i));
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		base->values[i] = matrix->values[i];
	struct Matrix* temp;
	if(exp < 0){
		exp = -exp;
		temp = matrix_inverse(base, invertible);
		matrix_free(base);
		if(!*invertible){
			matrix_free(result);
			return NULL;
		}
		base = temp;
	}
	while(exp > 0){
		if(exp % 2 == 1){
			temp = matrix_mul(result, base);
			matrix_free(result);
			result = temp;
		}
		temp = matrix_mul(base, base);
		matrix_free(base);
		base = temp;
		exp /= 2;
	}
	return result;
}

uint matrix_eq(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_neq(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] != matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_neq(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_eq(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] == matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_gt(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_le(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] <= matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_ge(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_lt(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] < matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_lt(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_ge(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] >= matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_le(struct Matrix* matrix1, struct Matrix* matrix2){
	uint i = 0;
#if HAVE_NEON
	for(; i + 4 <= matrix1->rows * matrix1->cols; i += 4)
		if(is_lanes_gt(vld1q_f32(matrix1->values + i), 
					vld1q_f32(matrix2->values)))
			return 0;
#endif
	for(; i < matrix1->rows * matrix1->cols; i++)
		if(matrix1->values[i] > matrix2->values[i])
			return 0;
	return 1;
}

uint matrix_eq_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_neq(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] != scalar)
			return 0;
	return 1;
}

uint matrix_neq_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_eq(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] == scalar)
			return 0;
	return 1;
}

uint matrix_gt_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_le(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] <= scalar)
			return 0;
	return 1;
}

uint matrix_ge_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_lt(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] < scalar)
			return 0;
	return 1;
}

uint matrix_lt_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_ge(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] >= scalar)
			return 0;
	return 1;
}

uint matrix_le_scalar(struct Matrix* matrix, float scalar){
	uint i = 0;
#if HAVE_NEON
	float32x4_t vscalar = vdupq_n_f32(scalar);
	for(; i + 4 <= matrix->rows * matrix->cols; i += 4)
		if(is_lanes_gt(vld1q_f32(matrix->values + i), vscalar))
			return 0;
#endif
	for(; i < matrix->rows * matrix->cols; i++)
		if(matrix->values[i] > scalar)
			return 0;
	return 1;
}
