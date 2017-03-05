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

#endif