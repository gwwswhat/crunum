#ifndef CRUNUM_VECTOR_H
#define CRUNUM_VECTOR_H

#include <lua5.4/lauxlib.h>

#include "common.h"

extern const luaL_Reg vector_functions[];
extern const luaL_Reg vector_methods[];

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

struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_mul_scalar(struct Vector* vector, float scalar);
struct Vector* vector_mul_matrix(struct Vector* vector, struct Matrix* matrix);
struct Vector* vector_add(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_add_scalar(struct Vector* vector, float scalar);

#endif
