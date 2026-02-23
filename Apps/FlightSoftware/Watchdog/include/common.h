#ifndef __WATCHDOG_COMMON_H__
#define __WATCHDOG_COMMON_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Some definitions to identify magic numbers in the code
#define BOOL bool
#define TRUE true
#define FALSE false

/**
 * Computes the number of elements in an array.
 */
#define SIZE_OF_ARRAY(ARRAY) (sizeof(ARRAY) / sizeof((ARRAY)[0]))

enum SLIP_Constants {
    SLIP_END = 0xC0,
    SLIP_ESC = 0xDB,
    SLIP_ESC_END = 0xDC,
    SLIP_ESC_ESC = 0xDD
};

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_COMMON_H__ */
