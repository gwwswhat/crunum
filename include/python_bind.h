#ifndef CRUNUM_PYTHON
#define CRUNUM_PYTHON

#include <Python.h>

#include "matrix.h"
#include "vector.h"

struct CrunumMatrix {
	PyObject_HEAD
	struct Matrix* matrix;
};

struct CrunumVector {
	PyObject_HEAD
	struct Vector* vector;
};

extern PyTypeObject crn_matrix_type;
extern PyModuleDef crn_matrix_def;
extern PyTypeObject crn_vector_type;
extern PyModuleDef crn_vector_def;

#endif
