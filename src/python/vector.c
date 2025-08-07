/*
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under GPL-3.0. See LICENSE for details.
 */

#pragma message "Python Vector"

#include "python.h"

static struct CrunumVector* crn_vector_new(PyObject* self, PyObject* args){
	(void)self;
	uint len;
	if(!PyArg_ParseTuple(args, "I", &len))
		return NULL;
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = vector_new(len);
	return crn_vector;
}

static struct CrunumVector* crn_vector_randinit(PyObject* self, PyObject* args){
	(void)self;
	uint len;
	if(!PyArg_ParseTuple(args, "I", &len))
		return NULL;
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = vector_randinit(len);
	return crn_vector;
}

static struct CrunumVector* crn_vector_from_list(PyObject* self, PyObject* args){
	(void)self;
	PyObject* list;
	if(!PyArg_ParseTuple(args, "O", &list))
		return NULL;
	if(!PyList_Check(list)){
		PyErr_SetString(PyExc_TypeError, "Expected a list");
		return NULL;
	}
	uint len = (uint)PyList_Size(list);
	struct CrunumVector* crn_vector = PyObject_New(struct CrunumVector, &crn_vector_type);
	if(!crn_vector)
		return NULL;
	crn_vector->vector = vector_new(len);
	for(uint i = 0; i < len; i++){
		PyObject* item = PyList_GetItem(list, i);
		if(!PyFloat_Check(item) && !PyLong_Check(item)){
			PyErr_SetString(PyExc_TypeError, "Vector elements must be float or integer");
			vector_free(crn_vector->vector);
			PyObject_Del(crn_vector);
			return NULL;
		}
		crn_vector->vector->values[i] = (float)PyFloat_AsDouble(item);
	}
	return crn_vector;
}

static PyObject* crn_vector_get_attro(PyObject* self, PyObject* attr_name){
	struct CrunumVector* crn_vector = (struct CrunumVector*)self;
	if(!PyUnicode_Check(attr_name)){
		PyErr_SetString(PyExc_TypeError, "Attribute name isn't a string");
		return NULL;
	}
	if(!PyUnicode_CompareWithASCIIString(attr_name, "len"))
		return PyLong_FromUnsignedLong((ulong)crn_vector->vector->len);
	return PyObject_GenericGetAttr(self, attr_name);
}

static PyObject* crn_vector_push(struct CrunumVector* self, PyObject* args){
	float value;
	if(!PyArg_ParseTuple(args, "f", &value))
		return NULL;
	vector_push(self->vector, value);
	Py_RETURN_NONE;
}

static PyObject* crn_vector_pop(struct CrunumVector* self, PyObject* noargs){
	(void)noargs;
	return PyFloat_FromDouble(vector_pop(self->vector));
}

static void crn_vector_free(struct CrunumVector* self){
	vector_free(self->vector);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* crn_vector_get(PyObject* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Index must be an integer");
		return NULL;
	}
	long index = PyLong_AsLong(key);
	struct CrunumVector* crn_vector = (struct CrunumVector*)self;
	if((uint)index >= crn_vector->vector->len || index < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	return PyFloat_FromDouble(crn_vector->vector->values[index]);
}

static int crn_vector_set(PyObject* self, PyObject* key, PyObject* value){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Index must be an integer");
		return -1;
	}
	long index = PyLong_AsLong(key);
	struct CrunumVector* crn_vector = (struct CrunumVector*)self;
	if((uint)index >= crn_vector->vector->len || index < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return -1;
	}
	if(!PyFloat_Check(value) && !PyLong_Check(value)){
		PyErr_SetString(PyExc_TypeError, "Value must be float or integer");
		return -1;
	}
	crn_vector->vector->values[index] = (float)PyFloat_AsDouble(value);
	return 0;
}

static PyObject* crn_vector_str(PyObject* self){
	struct Vector* vector = ((struct CrunumVector*)self)->vector;
	PyObject* result = PyUnicode_FromString("[");
	for(uint i = 0; i < vector->len; i++){
		char num[16];
		snprintf(num, sizeof(num), "%.2lf", vector->values[i]);
		PyUnicode_Append(&result, PyUnicode_FromString(num));
		if(i != vector->len - 1)
			PyUnicode_Append(&result, PyUnicode_FromString(", "));
	}
	PyUnicode_Append(&result, PyUnicode_FromString("]"));
	return result;
}

static PyObject* crn_vector_add(PyObject* left, PyObject* right){
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Vector* vector = ((struct CrunumVector*)right)->vector;
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_add_scalar(vector, scalar);
		return (PyObject*)result;
	}
	if(!PyObject_TypeCheck(left, &crn_vector_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Vector* vector1 = ((struct CrunumVector*)left)->vector;
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector2 = ((struct CrunumVector*)right)->vector;
		if(vector1->len != vector2->len){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match another vector length");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_add(vector1, vector2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_add_scalar(vector1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_vector_sub(PyObject* left, PyObject* right){
	if(!PyObject_TypeCheck(left, &crn_vector_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Vector* vector1 = ((struct CrunumVector*)left)->vector;
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector2 = ((struct CrunumVector*)right)->vector;
		if(vector1->len != vector2->len){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match another vector length");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_sub(vector1, vector2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_sub_scalar(vector1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_vector_mul(PyObject* left, PyObject* right){
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Vector* vector = ((struct CrunumVector*)right)->vector;
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_mul_scalar(vector, scalar);
		return (PyObject*)result;
	}
	if(!PyObject_TypeCheck(left, &crn_vector_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Vector* vector1 = ((struct CrunumVector*)left)->vector;
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector2 = ((struct CrunumVector*)right)->vector;
		if(vector1->len != vector2->len){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match another vector length");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_mul(vector1, vector2);
		return (PyObject*)result;
	}
	if(PyObject_TypeCheck(right, &crn_matrix_type)){
		struct Matrix* matrix = ((struct CrunumMatrix*)right)->matrix;
		if(vector1->len != matrix->rows){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match matrix row size");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_mul_matrix(vector1, matrix);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_mul_scalar(vector1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_vector_div(PyObject* left, PyObject* right){
	if(!PyObject_TypeCheck(left, &crn_vector_type))
		Py_RETURN_NOTIMPLEMENTED;
	struct Vector* vector1 = ((struct CrunumVector*)left)->vector;
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector2 = ((struct CrunumVector*)right)->vector;
		if(vector1->len != vector2->len){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match another vector length");
			return NULL;
		}
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_div(vector1, vector2);
		return (PyObject*)result;
	}
	if(PyFloat_Check(right) || PyLong_Check(right)){
		float scalar = (float)PyFloat_AsDouble(right);
		struct CrunumVector* result = PyObject_New(struct CrunumVector, &crn_vector_type);
		result->vector = vector_div_scalar(vector1, scalar);
		return (PyObject*)result;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyObject* crn_vector_compare(PyObject* left, PyObject* right, int op){
	uint cmp_result;
	if(PyFloat_Check(left) || PyLong_Check(left)){
		float scalar = (float)PyFloat_AsDouble(left);
		struct Vector* vector = ((struct CrunumVector*)right)->vector;
		switch(op){
			case Py_EQ:
				cmp_result = vector_eq_scalar(vector, scalar);
				break;
			case Py_NE:
				cmp_result = vector_neq_scalar(vector, scalar);
				break;
			case Py_LT:
				cmp_result = vector_lt_scalar(vector, scalar);
				break;
			case Py_LE:
				cmp_result = vector_le_scalar(vector, scalar);
				break;
			case Py_GT:
				cmp_result = vector_gt_scalar(vector, scalar);
				break;
			case Py_GE:
				cmp_result = vector_ge_scalar(vector, scalar);
				break;
			default:
				Py_RETURN_NOTIMPLEMENTED;
		}
		if(cmp_result)
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	struct Vector* vector1 = ((struct CrunumVector*)left)->vector;
	if(PyObject_TypeCheck(right, &crn_vector_type)){
		struct Vector* vector2 = ((struct CrunumVector*)right)->vector;
		if(vector1->len != vector2->len){
			PyErr_SetString(PyExc_ValueError, "Vector length doesn't match another vector length");
			return NULL;
		}
		switch(op){
			case Py_EQ:
				cmp_result = vector_eq(vector1, vector2);
				break;
			case Py_NE:
				cmp_result = vector_neq(vector1, vector2);
				break;
			case Py_LT:
				cmp_result = vector_lt(vector1, vector2);
				break;
			case Py_LE:
				cmp_result = vector_le(vector1, vector2);
				break;
			case Py_GT:
				cmp_result = vector_gt(vector1, vector2);
				break;
			case Py_GE:
				cmp_result = vector_ge(vector1, vector2);
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
				cmp_result = vector_eq_scalar(vector1, scalar);
				break;
			case Py_NE:
				cmp_result = vector_neq_scalar(vector1, scalar);
				break;
			case Py_LT:
				cmp_result = vector_lt_scalar(vector1, scalar);
				break;
			case Py_LE:
				cmp_result = vector_le_scalar(vector1, scalar);
				break;
			case Py_GT:
				cmp_result = vector_gt_scalar(vector1, scalar);
				break;
			case Py_GE:
				cmp_result = vector_ge_scalar(vector1, scalar);
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

PyMethodDef crn_vector_methods[] = {
	{"new", (PyCFunction)crn_vector_new, METH_VARARGS,
		"Params: len,\n"
		"Return: Vector,\n"
		"Desc: Create a new vector\n"
		"Example: crn.vector.new(10)"
	},
	{"randinit", (PyCFunction)crn_vector_randinit, METH_VARARGS,
		"Params: len,\n"
		"Return: Vector,\n"
		"Desc: Create a new randomized vector\n"
		"Example: crn.vector.randinit(10)"
	},
	{"from_list", (PyCFunction)crn_vector_from_list, METH_VARARGS,
		"Params: list,\n"
		"Return: Vector,\n"
		"Desc: Create a new vector based of the list given by the user\n"
		"Example: crn.vector.from_list([1, 2.3])"
	},
	{"push", (PyCFunction)crn_vector_push, METH_VARARGS,
		"Params: value,\n"
		"Return: None,\n"
		"Desc: Push value to vector\n"
		"Example: vec_var.push(5.5)"
	},
	{"pop", (PyCFunction)crn_vector_pop, METH_NOARGS,
		"Params: None,\n"
		"Return: float,\n"
		"Desc: Pop value from vector\n"
		"Example: vec_var.pop()"
	},
	{NULL, NULL, 0, NULL},
};

static PyMappingMethods crn_vector_as_mapping = {
	.mp_subscript = crn_vector_get,
	.mp_ass_subscript = crn_vector_set,
};

static PyNumberMethods crn_vector_as_number = {
	.nb_add = crn_vector_add,
	.nb_subtract = crn_vector_sub,
	.nb_multiply = crn_vector_mul,
	.nb_true_divide = crn_vector_div,
};

PyTypeObject crn_vector_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "crunum.vector.Vector",
	.tp_basicsize = sizeof(struct CrunumVector),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_dealloc = (destructor)crn_vector_free,
	.tp_methods = crn_vector_methods,
	.tp_str = crn_vector_str,
	.tp_as_mapping = &crn_vector_as_mapping,
	.tp_as_number = &crn_vector_as_number,
	.tp_richcompare = crn_vector_compare,
	.tp_getattro = crn_vector_get_attro,
};

PyModuleDef crn_vector_def = {
	PyModuleDef_HEAD_INIT,
	"vector",
	"Vector submodule",
	-1,
	crn_vector_methods,
	NULL,
	NULL,
	NULL,
	NULL,
};
