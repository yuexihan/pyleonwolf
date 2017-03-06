#ifndef leonwolf_LPSTree_h_included
#define leonwolf_LPSTree_h_included

#include "common.h"

typedef struct {
	PyObject_HEAD
	TYPE dtype;
	void* tree;
	void* lazy;
	int n;						// len(lpstree)
	int size;					// size of tree
} LPSTree;

static int getMid(int s, int e);
static void updateRangeUtil_int(int *tree, int *lazy, int nodeIndex, int segmentStart, int segmentEnd,
					int updateStart, int updateEnd, int difference);
static void updateRangeUtil_float(double *tree, double *lazy, int nodeIndex, int segmentStart, int segmentEnd,
					int updateStart, int updateEnd, double difference);
static void updateRange_int(int *tree, int *lazy, int n, int updateStart, int updateEnd, int difference);
static void updateRange_float(double *tree, double *lazy, int n, int updateStart, int updateEnd, double difference);
static int getSumUtil_int(int *tree, int *lazy, int segmentStart, int segmentEnd, int queryStart, int queryEnd, int nodeIndex);
static double getSumUtil_float(double *tree, double *lazy, int segmentStart, int segmentEnd, int queryStart, int queryEnd, int nodeIndex);
static int getSum_int(int *tree, int *lazy, int n, int queryStart, int queryEnd);
static double getSum_float(double *tree, double *lazy, int n, int queryStart, int queryEnd);
static int constructSTUtil_int(int* tree, int* lazy, int* array, int segmentStart, int segmentEnd, int nodeIndex);
static double constructSTUtil_float(double* tree, double* lazy, double* array,int segmentStart, int segmentEnd, int nodeIndex);
static void constructST_int(LPSTree* self, int* array);
static void constructST_float(LPSTree* self, double* array);
static PyObject* lpstree_new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static int lpstree_init(LPSTree* self, PyObject* args, PyObject* kwargs);
static void lpstree_del(LPSTree* self);
static PyObject* lpstree_update(LPSTree* self, PyObject* args);
static PyObject* lpstree_sum(LPSTree* self, PyObject* args);
static PyObject* lpstree_get(LPSTree* self, PyObject* args);
static PyObject* lpstree_set(LPSTree* self, PyObject* args);

#endif