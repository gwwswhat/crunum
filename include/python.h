#ifndef CRUNUM_PYTHON
#define CRUNUM_PYTHON

#include <Python.h>

#include "matrix.h"
#include "vector.h"

struct PyMatrix {
	PyObject_HEAD
	struct Matrix* matrix;
};

struct PyVector {
	PyObject_HEAD
	struct Vector* vector;
};

extern PyTypeObject py_matrix_type;
extern PyModuleDef py_matrix_def;
extern PyTypeObject py_vector_type;
extern PyModuleDef py_vector_def;

#endif
