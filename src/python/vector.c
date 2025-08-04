#include "python.h"

static PyObject* py_vector_new(PyObject* self, PyObject* args){
	(void)self;
	uint len;
	if(!PyArg_ParseTuple(args, "I", &len))
		return NULL;
	struct PyVector* py_vector = PyObject_New(struct PyVector, &py_vector_type);
	if(!py_vector)
		return NULL;
	py_vector->vector = vector_new(len);
	return (PyObject*)py_vector;
}

static void py_vector_free(struct PyVector* self){
	vector_free(self->vector);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* py_vector_get(PyObject* self, PyObject* key){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Index must be an integer");
		return NULL;
	}
	long index = PyLong_AsLong(key);
	struct PyVector* py_vector = (struct PyVector*)self;
	if((uint)index >= py_vector->vector->len || index < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return NULL;
	}
	return PyFloat_FromDouble(py_vector->vector->values[index]);
}

static int py_vector_set(PyObject* self, PyObject* key, PyObject* value){
	if(!PyLong_Check(key)){
		PyErr_SetString(PyExc_TypeError, "Index must be an integer");
		return -1;
	}
	long index = PyLong_AsLong(key);
	struct PyVector* py_vector = (struct PyVector*)self;
	if((uint)index >= py_vector->vector->len || index < 0){
		PyErr_SetString(PyExc_IndexError, "Out of bound");
		return -1;
	}
	if(!PyFloat_Check(value) && !PyLong_Check(value)){
		PyErr_SetString(PyExc_TypeError, "Value must be float or integer");
		return -1;
	}
	py_vector->vector->values[index] = (float)PyFloat_AsDouble(value);
	return 0;
}

PyMethodDef py_vector_methods[] = {
	{"new", py_vector_new, METH_VARARGS,
		"Params: len,\n"
		"Return: Vector,\n"
		"Create a new vector"
	},
	{NULL, NULL, 0, NULL},
};

static PyMappingMethods py_vector_type_as_mapping = {
	.mp_subscript = py_vector_get,
	.mp_ass_subscript = py_vector_set,
};

PyTypeObject py_vector_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "crunum.vector.Vector",
	.tp_basicsize = sizeof(struct PyVector),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_dealloc = (destructor)py_vector_free,
	.tp_methods = py_vector_methods,
	.tp_as_mapping = &py_vector_type_as_mapping,
};

PyModuleDef py_vector_def = {
	PyModuleDef_HEAD_INIT,
	"vector",
	"Vector submodule",
	-1,
	py_vector_methods,
	NULL,
	NULL,
	NULL,
	NULL,
};
