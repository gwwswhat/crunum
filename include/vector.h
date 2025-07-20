#ifndef CRUNUM_VECTOR_H
#define CRUNUM_VECTOR_H

#include <lua5.4/lauxlib.h>

#include "common.h"

extern const luaL_Reg vector_functions[];
extern const luaL_Reg vector_methods[];

struct Matrix;

struct Vector {
	double* values;
	uint len;
};

struct Vector* vector_init(uint len);
struct Vector* vector_randinit(uint len);
struct Vector* vector_from_matrix(struct Matrix* matrix);
void vector_free(struct Vector* vector);
struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_mul_scalar(struct Vector* vector, double scalar);
struct Vector* vector_sum(struct Vector* vector1, struct Vector* vector2);
struct Vector* vector_sum_scalar(struct Vector* vector, double scalar);

#endif
