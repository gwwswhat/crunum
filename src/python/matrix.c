/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#pragma message "Python Matrix"

#include "python.h"

static PyObject* py_matrix_new(PyObject* self, PyObject* args){
	(void)self;
	uint rows, cols;
	if(!PyArg_ParseTuple(args, "II", &rows, &cols))
		return NULL;
	struct PyMatrix* py_matrix = PyObject_New(struct PyMatrix, &py_matrix_type);
	if(!py_matrix)
		return NULL;
	py_matrix->matrix = matrix_new(rows, cols);
	return (PyObject*)py_matrix;
}

static PyObject* py_matrix_randinit(PyObject* self, PyObject* args){
	(void)self;
	uint rows, cols;
	if(!PyArg_ParseTuple(args, "II", &rows, &cols))
		return NULL;
	struct PyMatrix* py_matrix = PyObject_New(struct PyMatrix, &py_matrix_type);
	if(!py_matrix)
		return NULL;
	py_matrix->matrix = matrix_randinit(rows, cols);
	return (PyObject*)py_matrix;
}

static PyObject* py_matrix_from(PyObject* self, PyObject* args){
	(void)self;
	PyObject* outer_list;
	if(!PyArg_ParseTuple(args, "O", &outer_list))
		return NULL;
	if(!PyList_Check(outer_list)){
		PyErr_SetString(PyExc_TypeError, "Expected a 2D list");
		return NULL;
	}
	uint rows = (uint)PyList_Size(outer_list);
	uint cols = 0;
	uint first = 1;
	for(uint i = 0; i < rows; i++){
		PyObject* inner_list = PyList_GetItem(outer_list, i);
		if(!PyList_Check(inner_list)){
			PyErr_SetString(PyExc_TypeError, "Expected a list inside list");
			return NULL;
		}
		if(first)
			cols = (uint)PyList_Size(inner_list);
		else{
			if((uint)PyList_Size(inner_list) != cols){
				PyErr_SetString(PyExc_ValueError, "Inconsistent col size");
				return NULL;
			}
			first = 0;
		}
	}
	struct PyMatrix* py_matrix = PyObject_New(struct PyMatrix, &py_matrix_type);
	if(!py_matrix)
		return NULL;
	py_matrix->matrix = matrix_new(rows, cols);
	for(uint i = 0; i < rows; i++){
		PyObject* inner_list = PyList_GetItem(outer_list, i);
		for(uint j = 0; j < cols; j++){
			PyObject* item = PyList_GetItem(inner_list, j);
			if(!PyFloat_Check(item) && !PyLong_Check(item)){
				PyErr_SetString(PyExc_TypeError, "Matrix elements must be float or.int");
				matrix_free(py_matrix->matrix);
				PyObject_Del(py_matrix);
				return NULL;
			}
			matrix_set(py_matrix->matrix, i, j, (float)PyFloat_AsDouble(item));
		}
	}
	return (PyObject*)py_matrix;
}

static PyObject* py_matrix_identity(PyObject* self, PyObject* args){
	(void)self;
	uint size;
	if(!PyArg_ParseTuple(args, "I", &size))
		return NULL;
	struct PyMatrix* py_matrix = PyObject_New(struct PyMatrix, &py_matrix_type);
	if(!py_matrix)
		return NULL;
	py_matrix->matrix = matrix_identity(size);
	return (PyObject*)py_matrix;
}

static void py_matrix_free(struct PyMatrix* self){
	matrix_free(self->matrix);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* py_matrix_get(struct PyMatrix* self, PyObject* args){
	uint row, col;
	if(!PyArg_ParseTuple(args, "II", &row, &col))
		return NULL;
	if(row >= self->matrix->rows || col >= self->matrix->cols){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	return PyFloat_FromDouble((double)*matrix_get(self->matrix, row, col));
}

static PyObject* py_matrix_set(struct PyMatrix* self, PyObject* args){
	uint row, col;
	float value;
	if(!PyArg_ParseTuple(args, "IIf", &row, &col, &value))
		return NULL;
	if(row >= self->matrix->rows || col >= self->matrix->cols){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	matrix_set(self->matrix, row, col, value);
	Py_RETURN_NONE;
}

static PyObject* py_matrix_row(struct PyMatrix* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Matrix indices must be unsigned integer");
		return NULL;
	}
	int row = PyLong_AsLong(key);
	if((uint)row >= self->matrix->rows || row < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	struct PyVector* py_vector = PyObject_New(struct PyVector, &py_vector_type);
	if(!py_vector)
		return NULL;
	py_vector->vector = matrix_row(self->matrix, (uint)row);
	return (PyObject*)py_vector;
}

static PyObject* py_matrix_col(struct PyMatrix* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Matrix indices must be unsigned integer");
		return NULL;
	}
	int col = PyLong_AsLong(key);
	if((uint)col >= self->matrix->cols || col < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	struct PyVector* py_vector = PyObject_New(struct PyVector, &py_vector_type);
	if(!py_vector)
		return NULL;
	py_vector->vector = matrix_col(self->matrix, (uint)col);
	return (PyObject*)py_vector;
}

static PyObject* py_matrix_get_attro(PyObject* self, PyObject* attr_name){
	struct PyMatrix* py_matrix = (struct PyMatrix*)self;
	if(!PyUnicode_Check(attr_name)){
		PyErr_SetString(PyExc_TypeError, "Attribute name isn't a string");
		return NULL;
	}
	if(!PyUnicode_CompareWithASCIIString(attr_name, "rows"))
		return PyLong_FromUnsignedLong((ulong)py_matrix->matrix->rows);
	if(!PyUnicode_CompareWithASCIIString(attr_name, "cols"))
		return PyLong_FromUnsignedLong((ulong)py_matrix->matrix->rows);
	return PyObject_GenericGetAttr(self, attr_name);
}

static PyObject* py_matrix_transpose(struct PyMatrix* self, PyObject* noargs){
	(void)noargs;
	struct PyMatrix* result = PyObject_New(struct PyMatrix, &py_vector_type);
	result->matrix = matrix_transpose(self->matrix);
	return (PyObject*)result;
}

static PyObject* py_matrix_reshape(struct PyMatrix* self, PyObject* args){
	uint new_rows, new_cols;
	if(!PyArg_ParseTuple(args, "II", &new_rows, &new_cols))
		return NULL;
	if(new_rows * new_cols != self->matrix->rows * self->matrix->cols){
		PyErr_SetString(PyExc_ValueError, "New size is different with old one");
		return NULL;
	}
	matrix_reshape(self->matrix, new_rows, new_cols);
	Py_RETURN_NONE;
}

static PyObject* py_matrix_inverse(struct PyMatrix* self, PyObject* noargs){
	(void)noargs;
	if(self->matrix->rows != self->matrix->cols){
		PyErr_SetString(PyExc_ValueError, "Matrix isn't a square");
		return NULL;
	}
	uint invertible;
	matrix_inverse(self->matrix, &invertible);
	if(!invertible){
		PyErr_SetString(PyExc_ValueError, "Matrix can't be inversed");
		return NULL;
	}
	Py_RETURN_NONE;
}

static PyObject* py_matrix_push_row(struct PyMatrix* self, PyObject* args){
	PyObject* obj;
	if(!PyArg_ParseTuple(args, "O", &obj))
		return NULL;
	if(!PyObject_TypeCheck(obj, &py_matrix_type)){
		PyErr_SetString(PyExc_TypeError, "Expected a vector");
		return NULL;
	}
	struct PyVector* py_vector = (struct PyVector*)obj;
	self->matrix->cols = self->matrix->cols ? 
		self->matrix->cols : py_vector->vector->len;
	self->matrix->cols_cap = self->matrix->cols_cap ? 
		self->matrix->cols_cap : py_vector->vector->len;
	if(self->matrix->cols != py_vector->vector->len){
		PyErr_SetString(PyExc_ValueError, 
				"Matrix col size doesn't match vector length");
	}
	matrix_push_row(self->matrix, py_vector->vector);
	Py_RETURN_NONE;
}

static PyObject* py_matrix_push_col(struct PyMatrix* self, PyObject* args){
	PyObject* obj;
	if(!PyArg_ParseTuple(args, "O", &obj))
		return NULL;
	if(!PyObject_TypeCheck(obj, &py_matrix_type)){
		PyErr_SetString(PyExc_TypeError, "Expected a vector");
		return NULL;
	}
	struct PyVector* py_vector = (struct PyVector*)obj;
	self->matrix->rows = self->matrix->rows ? 
		self->matrix->rows : py_vector->vector->len;
	self->matrix->rows_cap = self->matrix->rows_cap ? 
		self->matrix->rows_cap : py_vector->vector->len;
	if(self->matrix->rows != py_vector->vector->len){
		PyErr_SetString(PyExc_ValueError, 
				"Matrix row size doesn't match vector length");
	}
	matrix_push_col(self->matrix, py_vector->vector);
	Py_RETURN_NONE;
}

static PyObject* py_matrix_str(PyObject* self){
	struct Matrix* matrix = ((struct PyMatrix*)self)->matrix;
	PyObject* result = PyUnicode_FromString("[");
	for(uint i = 0; i < matrix->rows; i++){
		PyUnicode_Append(&result, PyUnicode_FromString("\n  ["));
		for(uint j = 0; j < matrix->cols; j++){
			char num[16];
			snprintf(num, sizeof(num), "%.2lf", *matrix_get(matrix, i, j));
			PyUnicode_Append(&result, PyUnicode_FromString(num));
			if(j != matrix->cols - 1)
				PyUnicode_Append(&result, PyUnicode_FromString(", "));
		}
		PyUnicode_Append(&result, PyUnicode_FromString("]"));
		if(i != matrix->rows - 1)
			PyUnicode_Append(&result, PyUnicode_FromString(","));
	}
	if(matrix->rows)
		PyUnicode_Append(&result, PyUnicode_FromString("\n"));
	PyUnicode_Append(&result, PyUnicode_FromString("]"));
	return result;
}

PyMethodDef py_matrix_methods[] = {
	{"new", py_matrix_new, METH_VARARGS,
		"Params: rows, cols,\n"
		"Return: Matrix,\n"
		"Desc: Create a new matrix\n"
		"Example: crn.matrix.new(10, 10)"
	},
	{"randinit", py_matrix_randinit, METH_VARARGS,
		"Params: rows, cols,\n"
		"Return: Matrix,\n"
		"Desc: Create a new randomized matrix with range 0-1\n"
		"Example: crn.matrix.randinit(2, 10)"
	},
	{"from", py_matrix_from, METH_VARARGS,
		"Params: 2d list,\n"
		"Return: Matrix,\n"
		"Desc: Create a new matrix based of the 2d list given by the user\n"
		"Example: crn.matrix.from([[2, 2]])"
	},
	{"identity", py_matrix_identity, METH_VARARGS,
		"Params: size,\n"
		"Return: Matrix,\n"
		"Desc: Create a new identity matrix\n"
		"Example: crn.matrix.identity(10)"
	},
	{"get", (PyCFunction)py_matrix_get, METH_VARARGS,
		"Params: row, col,\n"
		"Return: float,\n"
		"Desc: Get element from matrix\n"
		"Example: mat_var.get(0, 0)"
	},
	{"set", (PyCFunction)py_matrix_set, METH_VARARGS,
		"Params: row, col, value,\n"
		"Return: None,\n"
		"Desc: Set matrix element to specified value\n"
		"Example: mat_var.set(0, 0, 2.2)"
	},
	{"row", (PyCFunction)py_matrix_row, METH_VARARGS,
		"Params: row,\n"
		"Return: Vector,\n"
		"Desc: Get row from matrix\n"
		"Example: mat_var.row(0)"
	},
	{"col", (PyCFunction)py_matrix_col, METH_VARARGS,
		"Params: col,\n"
		"Return: Vector,\n"
		"Desc: Get col from matrix\n"
		"Example: mat_var.col(2)"
	},
	{"transpose", (PyCFunction)py_matrix_transpose, METH_NOARGS,
		"Params: None,\n"
		"Return: Matrix,\n"
		"Desc: Transpose a matrix\n"
		"Example: mat_var.transpose()"
	},
	{"reshape", (PyCFunction)py_matrix_reshape, METH_VARARGS,
		"Params: new_rows, new_cols,\n"
		"Return: None,\n"
		"Desc: Reshape matrix\n"
		"Example: mat_var.reshape(10, 10)"
	},
	{"inverse", (PyCFunction)py_matrix_inverse, METH_NOARGS,
		"Params: None,\n"
		"Return: None,\n"
		"Desc: Inverse matrix\n"
		"Example: mat_var.inverse()"
	},
	{"push_row", (PyCFunction)py_matrix_push_row, METH_VARARGS,
		"Params: Vector,\n"
		"Return: None,\n"
		"Desc: Push vector as new row of matrix\n"
		"Example: mat_var.push_row(vec_var)"
	},
	{"push_col", (PyCFunction)py_matrix_push_col, METH_VARARGS,
		"Params: Vector,\n"
		"Return: None,\n"
		"Desc: Push vector as new col of matrix\n"
		"Example: mat_var.push_col(vec_var)"
	},
	{NULL, NULL, 0, NULL},
};

PyTypeObject py_matrix_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "crunum.matrix.Matrix",
	.tp_basicsize = sizeof(struct PyMatrix),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_dealloc = (destructor)py_matrix_free,
	.tp_methods = py_matrix_methods,
	.tp_str = py_matrix_str,
	.tp_as_mapping = NULL,
	.tp_getattro = py_matrix_get_attro,
};

PyModuleDef py_matrix_def = {
	PyModuleDef_HEAD_INIT,
	"matrix",
	"Matrix submodule",
	-1,
	py_matrix_methods,
	NULL,
	NULL,
	NULL,
	NULL,
};
