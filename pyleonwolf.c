#include "common.h"
#include "LPSTree.h"

// Code
#include "LPSTree.c"

// registration table
static PyMethodDef leonwolf_module_methods[] = {
	{NULL, NULL, 0, NULL}
};

#define leonwolf_doc "leonwolf module"

#ifdef PY3K
static struct PyModuleDef leonwolf_module = {
	PyModuleDef_HEAD_INIT,
	"leonwolf",
	leonwolf_doc,
	-1,
	leonwolf_module_methods
};
#endif

#ifdef PY3K
#define init_function PyInit_leonwolf
#define init_return(value) return (value)
#else
#define init_function initleonwolf
#define init_return(unused) return
#endif

PyMODINIT_FUNC init_function() {
	PyObject* module;


#ifdef PY3K
	module = PyModule_Create(&leonwolf_module);
#else
	module = Py_InitModule3("leonwolf", leonwolf_module_methods, leonwolf_doc);
#endif
	if (module == NULL)
		init_return(NULL);

	if (PyType_Ready(&lpstree_type) < 0) {
		Py_DECREF(module);
		init_return(NULL);
	} else {
		Py_INCREF(&lpstree_type);
		PyModule_AddObject(module, "LPSTree", (PyObject *)&lpstree_type);
	}

	init_return(module);

}