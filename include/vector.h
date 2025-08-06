#ifndef CRUNUM_VECTOR_H
#define CRUNUM_VECTOR_H

#include "common.h"

struct Matrix;

struct Vector {
	float* values;
	uint len;
	uint cap;
};

struct Vector* vector_new(uint len);
struct Vector* vector_randinit(uint len);
struct Vector* vector_from_matrix(struct Matrix* matrix);
void vector_free(struct Vector* vector);
void vector_push(struct Vector* vector, float value);
static inline float vector_pop(struct Vector* vector){
	return vector->values[--vector->len];
}

struct Vector* vector_add(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_add_scalar(struct Vector* vector, float scalar);
struct Vector* vector_sub(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_sub_scalar(struct Vector* vector, float scalar);
struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_mul_scalar(struct Vector* vector, float scalar);
struct Vector* vector_mul_matrix(struct Vector* vector, struct Matrix* matrix);
struct Vector* vector_div(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_div_scalar(struct Vector* vector, float scalar);
uint vector_eq(struct Vector* vector1, struct Vector* vector2);
uint vector_neq(struct Vector* vector1, struct Vector* vector2);
uint vector_gt(struct Vector* vector1, struct Vector* vector2);
uint vector_ge(struct Vector* vector1, struct Vector* vector2);
uint vector_lt(struct Vector* vector1, struct Vector* vector2);
uint vector_le(struct Vector* vector1, struct Vector* vector2);
uint vector_eq_scalar(struct Vector* vector, float scalar);
uint vector_neq_scalar(struct Vector* vector, float scalar);
uint vector_gt_scalar(struct Vector* vector, float scalar);
uint vector_ge_scalar(struct Vector* vector, float scalar);
uint vector_lt_scalar(struct Vector* vector, float scalar);
uint vector_le_scalar(struct Vector* vector, float scalar);

#endif
