#ifndef leonwolf_common_h_included__
#define leonwolf_common_h_included__

#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define PY3K
#endif

typedef enum {
	INT,
	DOUBLE
} TYPE;

#endif