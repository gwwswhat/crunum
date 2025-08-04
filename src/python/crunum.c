#include "python.h"

static struct PyModuleDef py_crunum_def = {
	PyModuleDef_HEAD_INIT,
  .m_name = "crunum",
	.m_doc = "Library for matrix and vector operations",
	.m_size = -1,
	.m_methods = NULL,
};

PyMODINIT_FUNC PyInit_crunum(void){
	PyObject* crunum = PyModule_Create(&py_crunum_def);
	if(!crunum)
		return NULL;
	if(PyModule_AddStringConstant(crunum, "__version__", VERSION) < 0){
		Py_DECREF(crunum);
		return NULL;
	}
	PyObject* matrix = PyModule_Create(&py_matrix_def);
	if(!matrix)
		return NULL;
	if(PyModule_AddObject(crunum, "matrix", matrix) < 0){
		Py_DECREF(matrix);
		Py_DECREF(crunum);
		return NULL;
	}
	if(!PyImport_AddModule("crunum.matrix")){
		Py_DECREF(crunum);
		return NULL;
	}
	PyObject* vector = PyModule_Create(&py_vector_def);
	if(!vector)
		return NULL;
	if(PyModule_AddObject(crunum, "vector", vector) < 0){
		Py_DECREF(vector);
		Py_DECREF(matrix);
		Py_DECREF(crunum);
		return NULL;
	}
	if(!PyImport_AddModule("crunum.vector")){
		Py_DECREF(crunum);
		return NULL;
	}
	if(PyType_Ready(&py_matrix_type) < 0)
		return NULL;
	Py_INCREF(&py_matrix_type);
	PyModule_AddObject(matrix, "Matrix", (PyObject*)&py_matrix_type);
	if(PyType_Ready(&py_vector_type) < 0)
		return NULL;
	Py_INCREF(&py_vector_type);
	PyModule_AddObject(vector, "Vector", (PyObject*)&py_vector_type);
	return crunum;
}
