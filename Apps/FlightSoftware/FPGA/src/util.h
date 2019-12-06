/**
 * Some common functions.
 */

#ifndef UTIL_H
#define UTIL_H

#include <limits>

#include <stdlib.h>
#include <float.h>
#include <stdint.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define EPSILON (1e-4)

int nearlyEqual(float a, float b);
int nearlyEqual(int a, int b);

template <typename T>
bool checkClose(const T *lhs, const T *rhs, size_t size) {
	for (size_t i = 0; i < size; ++i)
		if (!nearlyEqual(lhs[i], rhs[i]))
			return false;

    return true;
}

#endif /* UTIL_H */
