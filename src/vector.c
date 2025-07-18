#include "vector.h"

struct Vector* vector_init(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->values = calloc(1, len * sizeof(double));
	return vector;
}

struct Vector* vector_randinit(uint len){
	struct Vector* vector = malloc(sizeof(struct Vector));
	vector->len = len;
	vector->values = malloc(len * sizeof(double));
	for(uint i = 0; i < len; i++)
		vector->values[i] = (double)rand() / (double)RAND_MAX;
	return matrix;
}

struct Vector* vector_from_matrix(struct Matrix* matrix){
	struct Vector* vector = vector_init(matrix->cols);
	for(uint i = 0; i < vector->len; i++)
		vector->values[i] = matrix->values[i];
	return vector;
}

struct Vector* matrix_row(struct Matrix* matrix, uint row){
	struct Vector* vector = vector_init(matrix->cols);
	for(uint i = 0; i < vector->len; i++)
		vector->values[i] = matrix_get(matrix, row, i);
	return vector;
}

void vector_free(struct Vector* vector){
	free(vector->values);
	free(vector);
}

struct Vector* vector_mul(struct Vector* vector1, struct Vector* vector2){
	struct Vector* result = vector_init(vector1->len);
	for(uint i = 0; i < vector1->len; i++)
		result->values[i] = vector1->values[i] * vector2->values[i];
	return result;
}

struct Vector* vector_mul_scalar(struct Vector* vector, double scalar){
	struct Vector* result = vector_init(vector->len);
	for(uint i = 0; i < result->len; i++)
		result->values[i] = vector->values[i] * scalar;
	return result;
}

struct Vector* vector_mul_matrix(struct Vector* vector, struct Matrix* matrix){
