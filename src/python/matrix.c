/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#pragma message "Python Matrix"

#include "python.h"

static struct CrunumMatrix* crn_matrix_new(PyObject* self, PyObject* args){
	(void)self;
	uint rows, cols;
	if(!PyArg_ParseTuple(args, "II", &rows, &cols))
		return NULL;
	struct CrunumMatrix* crn_matrix = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
	if(!crn_matrix)
		return NULL;
	crn_matrix->matrix = matrix_new(rows, cols);
	return crn_matrix;
}

static struct CrunumMatrix* crn_matrix_randinit(PyObject* self, PyObject* args){
	(void)self;
	uint rows, cols;
	if(!PyArg_ParseTuple(args, "II", &rows, &cols))
		return NULL;
	struct CrunumMatrix* crn_matrix = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
	if(!crn_matrix)
		return NULL;
	crn_matrix->matrix = matrix_randinit(rows, cols);
	return crn_matrix;
}

static struct CrunumMatrix* crn_matrix_from_list(PyObject* self, PyObject* args){
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
	struct CrunumMatrix* crn_matrix = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
	if(!crn_matrix)
		return NULL;
	crn_matrix->matrix = matrix_new(rows, cols);
	for(uint i = 0; i < rows; i++){
		PyObject* inner_list = PyList_GetItem(outer_list, i);
		for(uint j = 0; j < cols; j++){
			PyObject* item = PyList_GetItem(inner_list, j);
			if(!PyFloat_Check(item) && !PyLong_Check(item)){
				PyErr_SetString(PyExc_TypeError, "Matrix elements must be float or.int");
				matrix_free(crn_matrix->matrix);
				PyObject_Del(crn_matrix);
				return NULL;
			}
			matrix_set(crn_matrix->matrix, i, j, (float)PyFloat_AsDouble(item));
		}
	}
	return crn_matrix;
}

static struct CrunumMatrix* crn_matrix_identity(PyObject* self, PyObject* args){
	(void)self;
	uint size;
	if(!PyArg_ParseTuple(args, "I", &size))
		return NULL;
	struct CrunumMatrix* crn_matrix = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
	if(!crn_matrix)
		return NULL;
	crn_matrix->matrix = matrix_identity(size);
	return crn_matrix;
}

static void crn_matrix_free(struct CrunumMatrix* self){
	matrix_free(self->matrix);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* crn_matrix_get(struct CrunumMatrix* self, PyObject* args){
	uint row, col;
	if(!PyArg_ParseTuple(args, "II", &row, &col))
		return NULL;
	if(row >= self->matrix->rows || col >= self->matrix->cols){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	return PyFloat_FromDouble((double)*matrix_get(self->matrix, row, col));
}

static PyObject* crn_matrix_set(struct CrunumMatrix* self, PyObject* args){
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

static PyObject* crn_matrix_row(struct CrunumMatrix* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Matrix indices must be unsigned integer");
		return NULL;
	}
	int row = PyLong_AsLong(key);
	if((uint)row >= self->matrix->rows || row < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = matrix_row(self->matrix, (uint)row);
	return (PyObject*)crn_vector;
}

static PyObject* crn_matrix_col(struct CrunumMatrix* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Matrix indices must be unsigned integer");
		return NULL;
	}
	int col = PyLong_AsLong(key);
	if((uint)col >= self->matrix->cols || col < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = matrix_col(self->matrix, (uint)col);
	return (PyObject*)crn_vector;
}

static PyObject* crn_matrix_get_attro(PyObject* self, PyObject* attr_name){
	struct CrunumMatrix* crn_matrix = (struct CrunumMatrix*)self;
	if(!PyUnicode_Check(attr_name)){
		PyErr_SetString(PyExc_TypeError, "Attribute name isn't a string");
		return NULL;
	}
	if(!PyUnicode_CompareWithASCIIString(attr_name, "rows"))
		return PyLong_FromUnsignedLong((ulong)crn_matrix->matrix->rows);
	if(!PyUnicode_CompareWithASCIIString(attr_name, "cols"))
		return PyLong_FromUnsignedLong((ulong)crn_matrix->matrix->rows);
	return PyObject_GenericGetAttr(self, attr_name);
}

static PyObject* crn_matrix_transpose(struct CrunumMatrix* self, PyObject* noargs){
	(void)noargs;
	struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_vector_type);
	result->matrix = matrix_transpose(self->matrix);
	return (PyObject*)result;
}

static PyObject* crn_matrix_reshape(struct CrunumMatrix* self, PyObject* args){
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

static PyObject* crn_matrix_inverse(struct CrunumMatrix* self, PyObject* noargs){
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

static PyObject* crn_matrix_push_row(struct CrunumMatrix* self, PyObject* args){
	PyObject* obj;
	if(!PyArg_ParseTuple(args, "O", &obj))
		return NULL;
	if(!PyObject_TypeCheck(obj, &crn_matrix_type)){
		PyErr_SetString(PyExc_TypeError, "Expected a vector");
		return NULL;
	}
	struct CrunumVector* crn_vector = (struct CrunumVector*)obj;
	self->matrix->cols = self->matrix->cols ? 
		self->matrix->cols : crn_vector->vector->len;
	self->matrix->cols_cap = self->matrix->cols_cap ? 
		self->matrix->cols_cap : crn_vector->vector->len;
	if(self->matrix->cols != crn_vector->vector->len){
		PyErr_SetString(PyExc_ValueError, 
				"Matrix col size doesn't match vector length");
	}
	matrix_push_row(self->matrix, crn_vector->vector);
	Py_RETURN_NONE;
}

static PyObject* crn_matrix_push_col(struct CrunumMatrix* self, PyObject* args){
	PyObject* obj;
	if(!PyArg_ParseTuple(args, "O", &obj))
		return NULL;
	if(!PyObject_TypeCheck(obj, &crn_matrix_type)){
		PyErr_SetString(PyExc_TypeError, "Expected a vector");
		return NULL;
	}
	struct CrunumVector* crn_vector = (struct CrunumVector*)obj;
	self->matrix->rows = self->matrix->rows ? 
		self->matrix->rows : crn_vector->vector->len;
	self->matrix->rows_cap = self->matrix->rows_cap ? 
		self->matrix->rows_cap : crn_vector->vector->len;
	if(self->matrix->rows != crn_vector->vector->len){
		PyErr_SetString(PyExc_ValueError, 
				"Matrix row size doesn't match vector length");
	}
	matrix_push_col(self->matrix, crn_vector->vector);
	Py_RETURN_NONE;
}

static PyObject* crn_matrix_pop_row(struct CrunumMatrix* self, PyObject* noargs){
	(void)noargs;
	if(!self->matrix->rows){
		PyErr_SetString(PyExc_ValueError, "Empty matrix");
		return NULL;
	}
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = matrix_pop_row(self->matrix);
	return (PyObject*)crn_vector;
}

static PyObject* crn_matrix_pop_col(struct CrunumMatrix* self, PyObject* noargs){
	(void)noargs;
	if(!self->matrix->cols){
		PyErr_SetString(PyExc_ValueError, "Empty matrix");
		return NULL;
	}
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = matrix_pop_col(self->matrix);
	return (PyObject*)crn_vector;
}

static PyObject* crn_matrix_str(PyObject* self){
	struct Matrix* matrix = ((struct CrunumMatrix*)self)->matrix;
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

static PyObject* crn_matrix_add(PyObject* left, PyObject* right){
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Matrix* matrix = ((struct CrunumMatrix*)right)->matrix;
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_add_scalar(matrix, scalar);
		return (PyObject*)result;
	}
	if(!PyObject_TypeCheck(left, &crn_matrix_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Matrix* matrix1 = ((struct CrunumMatrix*)left)->matrix;
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix2 = ((struct CrunumMatrix*)right)->matrix;
		if(matrix1->rows * matrix1->cols != matrix2->rows * matrix2->rows){
			PyErr_SetString(PyExc_ValueError, "Matrix size doesn't match another matrix size");
			return NULL;
		}
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_add(matrix1, matrix2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_add_scalar(matrix1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_matrix_sub(PyObject* left, PyObject* right){
	if(!PyObject_TypeCheck(left, &crn_matrix_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Matrix* matrix1 = ((struct CrunumMatrix*)left)->matrix;
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix2 = ((struct CrunumMatrix*)right)->matrix;
		if(matrix1->rows * matrix1->cols != matrix2->rows * matrix2->rows){
			PyErr_SetString(PyExc_ValueError, "Matrix size doesn't match another matrix size");
			return NULL;
		}
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_sub(matrix1, matrix2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_sub_scalar(matrix1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_matrix_mul(PyObject* left, PyObject* right){
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Matrix* matrix = ((struct CrunumMatrix*)right)->matrix;
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_mul_scalar(matrix, scalar);
		return (PyObject*)result;
	}
	if(!PyObject_TypeCheck(left, &crn_matrix_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Matrix* matrix1 = ((struct CrunumMatrix*)left)->matrix;
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix2 = ((struct CrunumMatrix*)right)->matrix;
		if(matrix1->rows != matrix2->cols){
			PyErr_SetString(PyExc_ValueError, "Matrix row size doesn't match another matrix col size");
			return NULL;
		}
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_mul(matrix1, matrix2);
		return (PyObject*)result;
	}
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector = ((struct CrunumVector*)right)->vector;
		if(matrix1->cols != vector->len){
			PyErr_SetString(PyExc_ValueError, "Matrix col size doesn't match vector length");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = matrix_mul_vector(matrix1, vector);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_mul_scalar(matrix1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_matrix_div(PyObject* left, PyObject* right){
	if(!PyObject_TypeCheck(left, &crn_matrix_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Matrix* matrix1 = ((struct CrunumMatrix*)left)->matrix;
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix2 = ((struct CrunumMatrix*)right)->matrix;
		if(matrix1->rows * matrix1->cols != matrix2->rows * matrix2->rows){
			PyErr_SetString(PyExc_ValueError, "Matrix size doesn't match another matrix size");
			return NULL;
		}
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_div(matrix1, matrix2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
		result->matrix = matrix_div_scalar(matrix1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_matrix_pow(PyObject* base, PyObject* exp, PyObject* mod){
	(void)mod;
	if(!PyObject_TypeCheck(base, &crn_matrix_type) ||
			(!PyFloat_Check(exp) && !PyLong_Check(exp)))
		Py_RETURN_NOTIMPLEMENTED;
	struct Matrix* matrix = ((struct CrunumMatrix*)base)->matrix;
	if(matrix->rows != matrix->cols){
		PyErr_SetString(PyExc_ValueError, "Matrix isn't a square");
		return NULL;
	}
	float scalar = (float)PyFloat_AsDouble(exp);
	struct CrunumMatrix* result = PyObject_New(struct CrunumMatrix, &crn_matrix_type);
	uint invertible;
	result->matrix = matrix_pow(matrix, scalar, &invertible);
	if(!invertible){
		PyErr_SetString(PyExc_ValueError, "Matrix can't be inversed");
		return NULL;
	}
	return (PyObject*)result;
}

static PyObject* crn_matrix_compare(PyObject* left, PyObject* right, int op){
	uint cmp_result;
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Matrix* matrix = ((struct CrunumMatrix*)right)->matrix;
		switch(op){
			case Py_EQ:
				cmp_result = matrix_eq_scalar(matrix, scalar);
				break;
			case Py_NE:
				cmp_result = matrix_neq_scalar(matrix, scalar);
				break;
			case Py_LT:
				cmp_result = matrix_lt_scalar(matrix, scalar);
				break;
			case Py_LE:
				cmp_result = matrix_le_scalar(matrix, scalar);
				break;
			case Py_GT:
				cmp_result = matrix_gt_scalar(matrix, scalar);
				break;
			case Py_GE:
				cmp_result = matrix_ge_scalar(matrix, scalar);
				break;
			default:
				Py_RETURN_NOTIMPLEMENTED;
		}
		if(cmp_result)
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	struct Matrix* matrix1 = ((struct CrunumMatrix*)left)->matrix;
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix2 = ((struct CrunumMatrix*)right)->matrix;
		if(matrix1->rows * matrix1->cols != matrix2->rows * matrix2->rows){
			PyErr_SetString(PyExc_ValueError, "Matrix size doesn't match another matrix size");
			return NULL;
		}
		switch(op){
			case Py_EQ:
				cmp_result = matrix_eq(matrix1, matrix2);
				break;
			case Py_NE:
				cmp_result = matrix_neq(matrix1, matrix2);
				break;
			case Py_LT:
				cmp_result = matrix_lt(matrix1, matrix2);
				break;
			case Py_LE:
				cmp_result = matrix_le(matrix1, matrix2);
				break;
			case Py_GT:
				cmp_result = matrix_gt(matrix1, matrix2);
				break;
			case Py_GE:
				cmp_result = matrix_ge(matrix1, matrix2);
				break;
			default:
				Py_RETURN_NOTIMPLEMENTED;
		}
		if(cmp_result)
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		switch(op){
			case Py_EQ:
				cmp_result = matrix_eq_scalar(matrix1, scalar);
				break;
			case Py_NE:
				cmp_result = matrix_neq_scalar(matrix1, scalar);
				break;
			case Py_LT:
				cmp_result = matrix_lt_scalar(matrix1, scalar);
				break;
			case Py_LE:
				cmp_result = matrix_le_scalar(matrix1, scalar);
				break;
			case Py_GT:
				cmp_result = matrix_gt_scalar(matrix1, scalar);
				break;
			case Py_GE:
				cmp_result = matrix_ge_scalar(matrix1, scalar);
				break;
			default:
				Py_RETURN_NOTIMPLEMENTED;
		}
		if(cmp_result)
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

PyMethodDef crn_matrix_methods[] = {
	{"new", (PyCFunction)crn_matrix_new, METH_VARARGS,
		"Params: rows, cols,\n"
		"Return: Matrix,\n"
		"Desc: Create a new matrix\n"
		"Example: crn.matrix.new(10, 10)"
	},
	{"randinit", (PyCFunction)crn_matrix_randinit, METH_VARARGS,
		"Params: rows, cols,\n"
		"Return: Matrix,\n"
		"Desc: Create a new randomized matrix with range 0-1\n"
		"Example: crn.matrix.randinit(2, 10)"
	},
	{"from_list", (PyCFunction)crn_matrix_from_list, METH_VARARGS,
		"Params: 2d list,\n"
		"Return: Matrix,\n"
		"Desc: Create a new matrix based of the 2d list given by the user\n"
		"Example: crn.matrix.from_list([[2, 2]])"
	},
	{"identity", (PyCFunction)crn_matrix_identity, METH_VARARGS,
		"Params: size,\n"
		"Return: Matrix,\n"
		"Desc: Create a new identity matrix\n"
		"Example: crn.matrix.identity(10)"
	},
	{"get", (PyCFunction)crn_matrix_get, METH_VARARGS,
		"Params: row, col,\n"
		"Return: float,\n"
		"Desc: Get element from matrix\n"
		"Example: mat_var.get(0, 0)"
	},
	{"set", (PyCFunction)crn_matrix_set, METH_VARARGS,
		"Params: row, col, value,\n"
		"Return: None,\n"
		"Desc: Set matrix element to specified value\n"
		"Example: mat_var.set(0, 0, 2.2)"
	},
	{"row", (PyCFunction)crn_matrix_row, METH_VARARGS,
		"Params: row,\n"
		"Return: Vector,\n"
		"Desc: Get row from matrix\n"
		"Example: mat_var.row(0)"
	},
	{"col", (PyCFunction)crn_matrix_col, METH_VARARGS,
		"Params: col,\n"
		"Return: Vector,\n"
		"Desc: Get col from matrix\n"
		"Example: mat_var.col(2)"
	},
	{"transpose", (PyCFunction)crn_matrix_transpose, METH_NOARGS,
		"Params: None,\n"
		"Return: Matrix,\n"
		"Desc: Transpose a matrix\n"
		"Example: mat_var.transpose()"
	},
	{"reshape", (PyCFunction)crn_matrix_reshape, METH_VARARGS,
		"Params: new_rows, new_cols,\n"
		"Return: None,\n"
		"Desc: Reshape matrix\n"
		"Example: mat_var.reshape(10, 10)"
	},
	{"inverse", (PyCFunction)crn_matrix_inverse, METH_NOARGS,
		"Params: None,\n"
		"Return: None,\n"
		"Desc: Inverse matrix\n"
		"Example: mat_var.inverse()"
	},
	{"push_row", (PyCFunction)crn_matrix_push_row, METH_VARARGS,
		"Params: Vector,\n"
		"Return: None,\n"
		"Desc: Push vector as a new row of matrix\n"
		"Example: mat_var.push_row(vec_var)"
	},
	{"push_col", (PyCFunction)crn_matrix_push_col, METH_VARARGS,
		"Params: Vector,\n"
		"Return: None,\n"
		"Desc: Push vector as a new col of matrix\n"
		"Example: mat_var.push_col(vec_var)"
	},
	{"pop_row", (PyCFunction)crn_matrix_pop_row, METH_NOARGS,
		"Params: None,\n"
		"Return: Vector,\n"
		"Desc: Pop a row from matrix\n"
		"Example: mat_var.pop_row()"
	},
	{"pop_col", (PyCFunction)crn_matrix_pop_col, METH_NOARGS,
		"Params: None,\n"
		"Return: Vector,\n"
		"Desc: Pop a col from matrix\n"
		"Example: mat_var.pop_col()"
	},
	{NULL, NULL, 0, NULL},
};

static PyNumberMethods crn_matrix_as_number = {
	.nb_add = crn_matrix_add,
	.nb_subtract = crn_matrix_sub,
	.nb_multiply = crn_matrix_mul,
	.nb_true_divide = crn_matrix_div,
	.nb_power = crn_matrix_pow,
};

PyTypeObject crn_matrix_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "crunum.matrix.Matrix",
	.tp_basicsize = sizeof(struct CrunumMatrix),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_dealloc = (destructor)crn_matrix_free,
	.tp_methods = crn_matrix_methods,
	.tp_str = crn_matrix_str,
	.tp_as_mapping = NULL,
	.tp_as_number = &crn_matrix_as_number,
	.tp_richcompare = crn_matrix_compare,
	.tp_getattro = crn_matrix_get_attro,
};

PyModuleDef crn_matrix_def = {
	PyModuleDef_HEAD_INIT,
	"matrix",
	"Matrix submodule",
	-1,
	crn_matrix_methods,
	NULL,
	NULL,
	NULL,
	NULL,
};
