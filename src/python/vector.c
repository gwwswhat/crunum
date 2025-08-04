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

static PyObject* py_vector_randinit(PyObject* self, PyObject* args){
	(void)self;
	uint len;
	if(!PyArg_ParseTuple(args, "I", &len))
		return NULL;
	struct PyVector* py_vector = PyObject_New(struct PyVector, &py_vector_type);
	if(!py_vector)
		return NULL;
	py_vector->vector = vector_randinit(len);
	return (PyObject*)py_vector;
}

static PyObject* py_vector_from(PyObject* self, PyObject* args){
	(void)self;
	PyObject* list;
	if(!PyArg_ParseTuple(args, "O", &list))
		return NULL;
	if(!PyList_Check(list)){
		PyErr_SetString(PyExc_TypeError, "Expected a list");
		return NULL;
	}
	uint len = (uint)PyList_Size(list);
	struct PyVector* py_vector = PyObject_New(struct PyVector, &py_vector_type);
	if(!py_vector)
		return NULL;
	py_vector->vector = vector_new(len);
	for(uint i = 0; i < len; i++){
		PyObject* item = PyList_GetItem(list, i);
		if(!PyFloat_Check(item) && !PyLong_Check(item)){
			PyErr_SetString(PyExc_TypeError, "Vector elements must be float or integer");
			vector_free(py_vector->vector);
			PyObject_Del(py_vector);
			return NULL;
		}
		py_vector->vector->values[i] = (float)PyFloat_AsDouble(item);
	}
	return (PyObject*)py_vector;
}

static PyObject* py_vector_get_attro(PyObject* self, PyObject* attr_name){
	struct PyVector* py_vector = (struct PyVector*)self;
	if(!PyUnicode_Check(attr_name)){
		PyErr_SetString(PyExc_TypeError, "Attribute name isn't a string");
		return NULL;
	}
	if(!PyUnicode_CompareWithASCIIString(attr_name, "len"))
		return PyLong_FromUnsignedLong((ulong)py_vector->vector->len);
	return PyObject_GenericGetAttr(self, attr_name);
}

static PyObject* py_vector_push(struct PyVector* self, PyObject* args){
	float value;
	if(!PyArg_ParseTuple(args, "f", &value))
		return NULL;
	vector_push(self->vector, value);
	Py_RETURN_NONE;
}

static PyObject* py_vector_pop(struct PyVector* self, PyObject* noargs){
	(void)noargs;
	return PyFloat_FromDouble(vector_pop(self->vector));
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

static PyObject* py_vector_str(PyObject* self){
	struct Vector* vector = ((struct PyVector*)self)->vector;
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

PyMethodDef py_vector_methods[] = {
	{"new", py_vector_new, METH_VARARGS,
		"Params: len,\n"
		"Return: Vector,\n"
		"Desc: Create a new vector\n"
		"Example: crn.vector.new(10)"
	},
	{"randinit", py_vector_randinit, METH_VARARGS,
		"Params: len,\n"
		"Return: Vector,\n"
		"Desc: Create a new randomized vector\n"
		"Example: crn.vector.randinit(10)"
	},
	{"from", py_vector_from, METH_VARARGS,
		"Params: list,\n"
		"Return: Vector,\n"
		"Desc: Create a new vector based of the list given by the user\n"
		"Example: crn.vector.from([1, 2.3])"
	},
	{"push", (PyCFunction)py_vector_push, METH_VARARGS,
		"Params: value,\n"
		"Return: None,\n"
		"Desc: Push value to vector\n"
		"Example: vec_var.push(5.5)"
	},
	{"pop", (PyCFunction)py_vector_pop, METH_NOARGS,
		"Params: None,\n"
		"Return: float,\n"
		"Desc: Pop value from vector\n"
		"Example: vec_var.pop()"
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
	.tp_str = py_vector_str,
	.tp_as_mapping = &py_vector_type_as_mapping,
	.tp_getattro = py_vector_get_attro,
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
