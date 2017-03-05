#include <common.h>
#include <math.h>

// get the middle index [s, e]
int getMid(int s, int e) {
	return s + (e - s) / 2;
}

void updateRangeUtil(int *tree, int *lazy, int nodeIndex, int segmentStart, int segmentEnd,
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
	updateRangeUtil(tree, lazy, 2*nodeIndex+1, segmentStart, mid, updateStart, updateEnd, difference);
	updateRangeUtil(tree, lazy, 2*nodeIndex+2, mid+1, segmentEnd, updateStart, updateEnd, difference);
	tree[nodeIndex] = tree[2*nodeIndex+1] + tree[2*nodeIndex+2];
}

void updateRange(int *tree, int *lazy, int n, int updateStart, int updateEnd, int difference) {
	updateRangeUtil(tree, lazy, 0, 0, n-1, updateStart, updateEnd, difference);
}

int getSumUtil(int *tree, int *lazy, int segmentStart, int segmentEnd, int queryStart, int queryEnd, int nodeIndex) {
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
	return getSumUtil(tree, lazy, segmentStart, mid, queryStart, queryEnd, 2*nodeIndex+1) + 
			getSumUtil(tree, lazy, mid+1, segmentEnd, queryStart, queryEnd, 2*nodeIndex+2);
}

int getSum(int *tree, int *lazy, int n, int queryStart, int queryEnd) {
	if (queryStart < 0 || queryEnd > n-1 || queryStart > queryEnd) {
		printf("Invalid Input");
		return;
	}
	return getSumUtil(tree, lazy, 0, n-1, queryStart, queryEnd, 0);
}

void getValue(int *tree, int *lazy, int n, int i) {
	return getSum(tree, lazy, n, i, i);
}

void updateValue(int *tree, int *lazy, int n, int i, int new_val) {
	int old_val = getValue(tree, lazy, n, i);
	int difference = new_val - old_val;
	updateRange(tree, lazy, n, i, i, difference);
}

int constructSTUtil(int arr[], int segmentStart, int segmentEnd, int *tree, int nodeIndex) {
	if (segmentStart == segmentEnd) {
		tree[nodeIndex] = arr[segmentStart];
		return arr[segmentStart];
	}
	int mid = getMid(segmentStart, segmentEnd);
	tree[nodeIndex] = constructSTUtil(arr, segmentStart, mid, tree, 2*nodeIndex+1) + 
			constructSTUtil(arr, mid+1, segmentEnd, tree, 2*nodeIndex+2);
	return tree[nodeIndex];
}

int *constructST(int arr[], int n) {
	int x = int(ceil(log2(n)));
	int max_size = 2*int(pow(2, x)) - 1;
	int *st = new int[max_size];
	constructSTUtil(arr, 0, n-1, st, 0);
	return st;
}


static PyObject* lpstree_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
	LPSTree* self = NULL;

	self = (LPSTree*)PyObject_New(LPSTree, type);
	if (self == NULL)
		return NULL;

	self.version = 0;
	self.count = 0;
}

static PyTypeObject lpstree_type = {
	PY_OBJECT_HEAD_INIT
	"leonwolf.LPSTree",
	sizeof(LPSTree),
}