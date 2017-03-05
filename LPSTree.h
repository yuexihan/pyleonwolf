#ifndef leonwolf_LPSTree_h_included
#define leonwolf_LPSTree_h_included

#include "common.h"

typedef struct {
	int Version;
	ssize_t nodes_count;		// total number of nodes in tree
	ssize_t numbers_count;		// len(lpstree)
	ssize_t links_count;		
	ssize_t sizeof_number;		// bytes to store one number
	ssize_t total_size;			// total size in bytes
} LPSTreeStatistics;

typedef struct {
	PyObject_HEAD
	int* tree;
	int* lazy;
	int count;					// len(lpstree)
	int version;
	LPSTreeStatistics stats;
} LPSTree;