#include "LPSTree.h"
#include <math.h>

static PyTypeObject lpstree_type;

#define lpstree_doc \
	"LPSTree(n[, dtype])\n"\
	"LPSTree(lpstree)\n"\
	"\n"\
	"LPSTree(n[, dtype]) create a Lazy Propagation Tree with all elements set to zero.\n"\
	"Dtype can be int or float.\n\n"\
	"LPSTree(lpstree) create a copy of original tree."

// get the middle index [s, e]
static int getMid(int s, int e) {
	return s + (e - s) / 2;
}

static void updateRangeUtil_int(int *tree, int *lazy, int nodeIndex, int segmentStart, int segmentEnd,
					int updateStart, int updateEnd, int difference) {
	if (lazy[nodeIndex] != 0) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * lazy[nodeIndex];
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += lazy[nodeIndex];
			lazy[2 * nodeIndex + 2] += lazy[nodeIndex];
		}
		lazy[nodeIndex] = 0;
	}
	if (segmentStart > segmentEnd || segmentStart > updateEnd || segmentEnd < updateStart) {
		return;
	}
	if (segmentStart >= updateStart && segmentEnd <= updateEnd) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * difference;
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += difference;
			lazy[2 * nodeIndex + 2] += difference;
		}
		return;
	}
	int mid = getMid(segmentStart, segmentEnd);
	updateRangeUtil_int(tree, lazy, 2*nodeIndex+1, segmentStart, mid, updateStart, updateEnd, difference);
	updateRangeUtil_int(tree, lazy, 2*nodeIndex+2, mid+1, segmentEnd, updateStart, updateEnd, difference);
	tree[nodeIndex] = tree[2*nodeIndex+1] + tree[2*nodeIndex+2];
}

static void updateRangeUtil_float(double *tree, double *lazy, int nodeIndex, int segmentStart, int segmentEnd,
					int updateStart, int updateEnd, double difference) {
	if (lazy[nodeIndex] != 0) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * lazy[nodeIndex];
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += lazy[nodeIndex];
			lazy[2 * nodeIndex + 2] += lazy[nodeIndex];
		}
		lazy[nodeIndex] = 0;
	}
	if (segmentStart > segmentEnd || segmentStart > updateEnd || segmentEnd < updateStart) {
		return;
	}
	if (segmentStart >= updateStart && segmentEnd <= updateEnd) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * difference;
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += difference;
			lazy[2 * nodeIndex + 2] += difference;
		}
		return;
	}
	int mid = getMid(segmentStart, segmentEnd);
	updateRangeUtil_float(tree, lazy, 2*nodeIndex+1, segmentStart, mid, updateStart, updateEnd, difference);
	updateRangeUtil_float(tree, lazy, 2*nodeIndex+2, mid+1, segmentEnd, updateStart, updateEnd, difference);
	tree[nodeIndex] = tree[2*nodeIndex+1] + tree[2*nodeIndex+2];
}

static void updateRange_int(int *tree, int *lazy, int n, int updateStart, int updateEnd, int difference) {
	if (updateStart < 0 || updateEnd > n-1 || updateStart > updateEnd) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return;
	}
	updateRangeUtil_int(tree, lazy, 0, 0, n-1, updateStart, updateEnd, difference);
}

static void updateRange_float(double *tree, double *lazy, int n, int updateStart, int updateEnd, double difference) {
	if (updateStart < 0 || updateEnd > n-1 || updateStart > updateEnd) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return;
	}
	updateRangeUtil_float(tree, lazy, 0, 0, n-1, updateStart, updateEnd, difference);
}

static int getSumUtil_int(int *tree, int *lazy, int segmentStart, int segmentEnd, int queryStart, int queryEnd, int nodeIndex) {
	if (lazy[nodeIndex] != 0) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * lazy[nodeIndex];
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += lazy[nodeIndex];
			lazy[2 * nodeIndex + 2] += lazy[nodeIndex];
		}
		lazy[nodeIndex] = 0;
	}
	if (queryStart <= segmentStart && queryEnd >= segmentEnd)
		return tree[nodeIndex];
	if (segmentStart > segmentEnd || segmentEnd < queryStart || segmentStart > queryEnd)
		return 0;
	int mid = getMid(segmentStart, segmentEnd);
	return getSumUtil_int(tree, lazy, segmentStart, mid, queryStart, queryEnd, 2*nodeIndex+1) + 
			getSumUtil_int(tree, lazy, mid+1, segmentEnd, queryStart, queryEnd, 2*nodeIndex+2);
}

static double getSumUtil_float(double *tree, double *lazy, int segmentStart, int segmentEnd, int queryStart, int queryEnd, int nodeIndex) {
	if (lazy[nodeIndex] != 0) {
		tree[nodeIndex] += (segmentEnd - segmentStart + 1) * lazy[nodeIndex];
		if (segmentStart != segmentEnd) {
			lazy[2 * nodeIndex + 1] += lazy[nodeIndex];
			lazy[2 * nodeIndex + 2] += lazy[nodeIndex];
		}
		lazy[nodeIndex] = 0;
	}
	if (queryStart <= segmentStart && queryEnd >= segmentEnd)
		return tree[nodeIndex];
	if (segmentStart > segmentEnd || segmentEnd < queryStart || segmentStart > queryEnd)
		return 0;
	int mid = getMid(segmentStart, segmentEnd);
	return getSumUtil_float(tree, lazy, segmentStart, mid, queryStart, queryEnd, 2*nodeIndex+1) + 
			getSumUtil_float(tree, lazy, mid+1, segmentEnd, queryStart, queryEnd, 2*nodeIndex+2);
}

static int getSum_int(int *tree, int *lazy, int n, int queryStart, int queryEnd) {
	if (queryStart < 0 || queryEnd > n-1 || queryStart > queryEnd) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return 0;
	}
	return getSumUtil_int(tree, lazy, 0, n-1, queryStart, queryEnd, 0);
}

static double getSum_float(double *tree, double *lazy, int n, int queryStart, int queryEnd) {
	if (queryStart < 0 || queryEnd > n-1 || queryStart > queryEnd) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return 0;
	}
	return getSumUtil_float(tree, lazy, 0, n-1, queryStart, queryEnd, 0);
}

static int constructSTUtil_int(int* tree, int* lazy, int* array, int segmentStart, int segmentEnd, int nodeIndex) {
	lazy[nodeIndex] = 0;
	if (segmentStart == segmentEnd) {
		tree[nodeIndex] = array[segmentStart];
		return array[segmentStart];
	}
	int mid = getMid(segmentStart, segmentEnd);
	tree[nodeIndex] = constructSTUtil_int(tree, lazy, array, segmentStart, mid, 2*nodeIndex+1) + 
			constructSTUtil_int(tree, lazy, array, mid+1, segmentEnd, 2*nodeIndex+2);
	return tree[nodeIndex];
}

static double constructSTUtil_float(double* tree, double* lazy, double* array,int segmentStart, int segmentEnd, int nodeIndex) {
	lazy[nodeIndex] = 0.0;
	if (segmentStart == segmentEnd) {
		tree[nodeIndex] = array[segmentStart];
		return array[segmentStart];
	}
	int mid = getMid(segmentStart, segmentEnd);
	tree[nodeIndex] = constructSTUtil_float(tree, lazy, array, segmentStart, mid, 2*nodeIndex+1) + 
			constructSTUtil_float(tree, lazy, array, mid+1, segmentEnd, 2*nodeIndex+2);
	return tree[nodeIndex];
}

static void constructST_int(LPSTree* self, int* array) {
	int n = self->n;
	int x = (int)ceil(log2(n));
	int size = 2*(int)pow(2, x) - 1;
	self->size = size;

	int* tree = PyMem_New(int, size);
	int* lazy = PyMem_New(int, size);
	if (tree == NULL || lazy == NULL) {
		return PyErr_NoMemory();
	}
	constructSTUtil_int(tree, lazy, array, 0, n-1, 0);
	self->tree = tree;
	self->lazy = lazy;
}
static void constructST_float(LPSTree* self, double* array) {
	int n = self->n;
	int x = (int)ceil(log2(n));
	int size = 2*(int)pow(2, x) - 1;
	self->size = size;

	double* tree = PyMem_New(double, size);
	double* lazy = PyMem_New(double, size);
	if (tree == NULL || lazy == NULL) {
		return PyErr_NoMemory();
	}
	constructSTUtil_float(tree, lazy, array, 0, n-1, 0);
	self->tree = tree;
	self->lazy = lazy;
}


static PyObject* lpstree_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
	LPSTree* self = NULL;

	self = (LPSTree*)PyObject_New(LPSTree, type);
	if (self == NULL)
		return NULL;

	self->tree = NULL;
	self->lazy = NULL;
	return (PyObject*)self;
}

static int lpstree_init(LPSTree* self, PyObject* args, PyObject* kwargs) {
	PyObject* dtype = NULL;
	PyObject* object = NULL;
	int n = 0;
	char *kwlist[] = {"n", "dtype", NULL};
	if (PyArg_ParseTupleAndKeywords(args, kwargs, "i|O", kwlist, &n, &dtype)) {
		if (n <= 0) {
			PyErr_SetString(PyExc_ValueError, "Given n must be greater than 0.");
			return -1;
		}
		if (dtype != NULL) {
			if (!PyType_Check(dtype)) {
				PyErr_SetString(PyExc_TypeError, "Given type is not a type.");
				return -1;
			}
			if (dtype != &PyFloat_Type && dtype != &PyInt_Type) {
				PyErr_SetString(PyExc_ValueError, "Type can only be int or float.");
				return -1;
			}
		} else {
			dtype = &PyInt_Type;
		}
		self->n = n;
		if (dtype == &PyInt_Type) {
			self->dtype = INT;
			int* array = PyMem_New(int, n);
			if (array == NULL) {
				return PyErr_NoMemory();
				return -1;
			}
			int i;
			for (i = 0; i < n; i++) {
				array[i] = 0;
			}
			constructST_int(self, array);
			PyMem_Del(array);
			if (PyErr_Occurred()) {
				return -1;
			}
		} else {
			self->dtype = DOUBLE;
			double* array = PyMem_New(double, n);
			if (array == NULL) {
				return PyErr_NoMemory();
				return -1;
			}
			int i;
			for (i = 0; i < n; i++) {
				array[i] = 0.0;
			}
			constructST_float(self, array);
			PyMem_Del(array);
			if (PyErr_Occurred()) {
				return -1;
			}
		}
	} else if (PyArg_ParseTuple(args, "O", &object)) {
		PyErr_Clear();
		if (PyObject_IsInstance(object, &lpstree_type) != 1) {
			PyErr_SetString(PyExc_TypeError, "Given object is not an instance of LPSTree.");
			return -1;
		}
		LPSTree* targetObject = (LPSTree*)object;
		self->n = targetObject->n;
		self->dtype = targetObject->dtype;
		self->size = targetObject->size;
		if (self->dtype == INT) {
			int* tree = PyMem_New(int, self->size);
			int* lazy = PyMem_New(int, self->size);
			if (tree == NULL || lazy == NULL) {
				PyErr_NoMemory();
				return -1;
			}
			int* target_tree = (int*)targetObject->tree;
			int* target_lazy = (int*)targetObject->lazy;
			int i;
			for (i=0; i<self->size; i++) {
				tree[i] = target_tree[i];
				lazy[i] = target_lazy[i];
			}
			self->tree = tree;
			self->lazy = lazy;
		} else {
			double* tree = PyMem_New(double, self->size);
			double* lazy = PyMem_New(double, self->size);
			if (tree == NULL || lazy == NULL) {
				PyErr_NoMemory();
				return -1;
			}
			double* target_tree = (double*)targetObject->tree;
			double* target_lazy = (double*)targetObject->lazy;
			int i;
			for (i=0; i<self->size; i++) {
				tree[i] = target_tree[i];
				lazy[i] = target_lazy[i];
			}
			self->tree = tree;
			self->lazy = lazy;
		}

	} else {
		return -1;
	}
	return 0;
}

static void lpstree_del(LPSTree* self) {
	PyMem_Del(self->tree);
	PyMem_Del(self->lazy);
	PyObject_Del(self);
}

#define lpstree_update_doc \
	"lpstree.update(i, j, diff)\n"\
	"\n"\
	"Update range(i,j) by adding each element by diff.\n"\
	"The slice doesn't contain j that is [i, j) in interval notation."
static PyObject* lpstree_update(LPSTree* self, PyObject* args) {
	int n = self->n;
	if (self->dtype == INT) {
		int updateStart, updateEnd;
		int difference;
		if (!PyArg_ParseTuple(args, "iii", &updateStart, &updateEnd, &difference)) {
			PyErr_SetString(PyExc_ValueError, "Arguments error.");
			return NULL;
		}
		if (updateStart < 0 || updateEnd > n || updateStart > updateEnd) {
			PyErr_SetString(PyExc_IndexError, "Index out of range");
			return NULL;
		}
		int* tree = (int*)self->tree;
		int* lazy = (int*)self->lazy;
		updateRange_int(tree, lazy, n, updateStart, updateEnd-1, difference);
	} else {
		int updateStart, updateEnd;
		double difference;
		if (!PyArg_ParseTuple(args, "iid", &updateStart, &updateEnd, &difference)) {
			PyErr_SetString(PyExc_ValueError, "Arguments error.");
			return NULL;
		}
		if (updateStart < 0 || updateEnd > n || updateStart > updateEnd) {
			PyErr_SetString(PyExc_IndexError, "Index out of range");
			return NULL;
		}
		double* tree = (double*)self->tree;
		double* lazy = (double*)self->lazy;
		updateRange_float(tree, lazy, n, updateStart, updateEnd-1, difference);
	}
	Py_RETURN_NONE;
}

#define lpstree_sum_doc \
	"lpstree.sum(i, j) -> value\n"\
	"lpstree.sum(j) -> value\n"\
	"lpstree.sum() -> value\n"\
	"\n"\
	"lpstree.sum(i, j) returns sum of elements in range(i, j).\n"\
	"The slice doesn't contain j that is [i, j) in interval notation.\n"\
	"lpstree.sum(j) is just sum(0, j).\n"\
	"lpstree.sum() returns sum of all elements."
static PyObject* lpstree_sum(LPSTree* self, PyObject* args) {
	int n = self->n;
	int queryStart=0, queryEnd=n;
	if (PyTuple_Size(args) == 0) {;}
	else if (PyTuple_Size(args) == 1) {
		if (!PyArg_ParseTuple(args, "i", &queryEnd)) {
			PyErr_SetString(PyExc_ValueError, "Arguments error.");
			return NULL;
		}
	} else if (!PyArg_ParseTuple(args, "ii", &queryStart, &queryEnd)) {
		PyErr_SetString(PyExc_ValueError, "Arguments error.");
		return NULL;
	}
	if (queryStart < 0 || queryEnd > n || queryStart > queryEnd) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return NULL;
	}
	if (self->dtype == INT) {
		int* tree = (int*)self->tree;
		int* lazy = (int*)self->lazy;
		return PyInt_FromLong(getSum_int(tree, lazy, n, queryStart, queryEnd-1));
	} else {
		double* tree = (double*)self->tree;
		double* lazy = (double*)self->lazy;
		return PyFloat_FromDouble(getSum_float(tree, lazy, n, queryStart, queryEnd-1));
	}
}

#define lpstree_get_doc \
	"lpstree.get(i) -> item\n"\
	"\n"\
	"Get the element in position i."
static PyObject* lpstree_get(LPSTree* self, PyObject* args) {
	int n = self->n, i;
	if (!PyArg_ParseTuple(args, "i", &i)) {
		PyErr_SetString(PyExc_ValueError, "Arguments error.");
		return NULL;
	}
	if (i < 0 || i > n-1) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
		return NULL;
	}
	if (self->dtype == INT) {
		int* tree = (int*)self->tree;
		int* lazy = (int*)self->lazy;
		return PyInt_FromLong(getSum_int(tree, lazy, n, i, i));
	} else {
		double* tree = (double*)self->tree;
		double* lazy = (double*)self->lazy;
		return PyFloat_FromDouble(getSum_float(tree, lazy, n, i, i));
	}
}

#define lpstree_set_doc \
	"lpstree.set(i, value)\n"\
	"\n"\
	"Set the element in position i to value."
static PyObject* lpstree_set(LPSTree* self, PyObject* args) {
	int n = self->n, i;
	if (self->dtype == INT) {
		int* tree = (int*)self->tree;
		int* lazy = (int*)self->lazy;
		int newValue, oldValue, difference;
		if (!PyArg_ParseTuple(args, "ii", &i, &newValue)) {
			PyErr_SetString(PyExc_ValueError, "Arguments error.");
			return NULL;
		}
		if (i < 0 || i > n-1) {
			PyErr_SetString(PyExc_IndexError, "Index out of range");
			return NULL;
		}
		oldValue = getSum_int(tree, lazy, n, i, i);
		difference = newValue - oldValue;
		updateRange_int(tree, lazy, n, i, i, difference);
	} else {
		double* tree = (double*)self->tree;
		double* lazy = (double*)self->lazy;
		double newValue, oldValue, difference;
		if (!PyArg_ParseTuple(args, "id", &i, &newValue)) {
			PyErr_SetString(PyExc_ValueError, "Arguments error.");
			return NULL;
		}
		if (i < 0 || i > n-1) {
			PyErr_SetString(PyExc_IndexError, "Index out of range");
			return NULL;
		}
		oldValue = getSum_float(tree, lazy, n, i, i);
		difference = newValue - oldValue;
		updateRange_float(tree, lazy, n, i, i, difference);
	}
	Py_RETURN_NONE;
}

static ssize_t lpstree_Size(LPSTree* self) {
	return self->n;
}

static PyObject* lpstree_GetItem(LPSTree* self, ssize_t i) {
	PyObject* index = PyInt_FromLong(i);
	PyObject* new_args = Py_BuildValue("(O)", index);
	PyObject* result = lpstree_get(self, new_args);
	Py_DECREF(index);
	Py_DECREF(new_args);
	return result;
}

static int lpstree_SetItem(LPSTree* self, ssize_t i, PyObject* value) {
	PyObject* index = PyInt_FromLong(i);
	PyObject* new_args = Py_BuildValue("(OO)", index, value);
	lpstree_set(self, new_args);
	Py_DECREF(index);
	Py_DECREF(new_args);
	if (PyErr_Occurred()) {
		return -1;
	}
	return 0;
}

static PyMemberDef lpstree_members[] = {
	{
		"size",
		T_INT,
		offsetof(LPSTree, size),
		READONLY,
		"Read-only attribute. The size of segment tree."
	},
	{
		"n",
		T_INT,
		offsetof(LPSTree, n),
		READONLY,
		"Read-only attribute. Elements number in the tree."
	},
	{NULL}
};

static PyMethodDef lpstree_methods[] = {
	{"update", lpstree_update, METH_VARARGS, lpstree_update_doc},
	{"sum", lpstree_sum, METH_VARARGS, lpstree_sum_doc},
	{"get", lpstree_get, METH_VARARGS, lpstree_get_doc},
	{"set", lpstree_set, METH_VARARGS, lpstree_set_doc},
	{NULL, NULL, 0, NULL}
};

static PyTypeObject lpstree_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"leonwolf.LPSTree",
	sizeof(LPSTree),					// tp_size
	0,									// tp_itemsize?
	(destructor)lpstree_del,			// tp_dealloc
	0,									// tp_print
	0,									// tp_getattr
	0,									// tp_setattr
	0,									// tp_reserved
	0,									// tp_repr
	0,									// tp_as_number
	0,									// tp_as_sequence
	0,									// tp_as_mapping
	0,									// tp_hash
	0,									// tp_call
	0,									// tp_str
	0,									// tp_getattro
	0,									// tp_setattro
	0,									// tp_as_buffer
	Py_TPFLAGS_DEFAULT,					// tp_flags
	lpstree_doc,						// tp_doc
	0,									// tp_traverse
	0,									// tp_clear
	0,									// tp_richcompare
	0,									// tp_weaklistoffset
	0,									// tp_iter
	0,									// tp_iternext
	lpstree_methods,					// tp_methods
	lpstree_members,					// tp_members
	0,									// tp_getset
	0,									// tp_base
	0,									// tp_dict
	0,									// tp_descr_get
	0,									// tp_descr_set
	0,									// tp_dictoffset
	lpstree_init,						// tp_init
	0,									// tp_alloc
	lpstree_new,						// tp_new
};

static PySequenceMethods lpstree_as_sequence;