/**
 * Definition of util functions
 */

#include <math.h>
#include <float.h>
#include <limits>

#include "util.h"

int nearlyEqual(float a, float b) {
	float absA = fabs(a);
	float absB = fabs(b);
	float diff = fabs(a - b);

	if (a == b) { // shortcut, handles infinities
		return 1;
	} else if (a == 0 || b == 0 || diff < FLT_MIN) {
		// a or b is zero or both are extremely close to it
		// relative error is less meaningful here
		return diff < (EPSILON * FLT_MIN);
	} else { // use relative error
		return diff / fmin((absA + absB), FLT_MAX) < EPSILON;
	}
}

int nearlyEqual(int a, int b) {
	return (a == b);
}
