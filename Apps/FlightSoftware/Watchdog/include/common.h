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

#ifdef ENABLE_DEBUG_ONLY_CODE

#define XSTR(s) STR(s)
#define STR(s) #s

#include <stdio.h>
#define DPRINTF(...) printf(__VA_ARGS__)
#define DPRINTF_ERR(...) fprintf(stderr, __VA_ARGS__)

#define DEBUG_LOG_NULL_CHECK(variable, msg) \
    if (NULL == (variable)) { \
        DPRINTF_ERR("[%s:%d] %s: %s is NULL\n", __FILE__, __LINE__, (msg), STR(variable)); \
    }

#define DEBUG_LOG_CHECK_STATUS(expected, actual, msg) \
    if ((expected) != (actual)) { \
        DPRINTF_ERR("[%s:%d] %s: expected = %d, actual = %d\n", __FILE__, __LINE__, (msg), STR(variable)); \
    }

#else

#define XSTR(s)
#define STR(s)
#define DPRINTF(...)
#define DPRINTF_ERR(...)
#define DEBUG_LOG_NULL_CHECK(variable, msg)
#define DEBUG_LOG_CHECK_STATUS(expected, actual, msg)

#endif

#define DEBUG_LOG_NULL_CHECK_RETURN(variable, msg, retval) \
    if (NULL == (variable)) { \
        DPRINTF_ERR("[%s:%d] %s: %s is NULL\n", __FILE__, __LINE__, (msg), STR(variable)); \
        return (retval); \
    }

#define DEBUG_LOG_CHECK_STATUS_RETURN(expected, actual, msg, retval) \
    if ((expected) != (actual)) { \
        DPRINTF_ERR("[%s:%d] %s: expected = %d, actual = %d\n", __FILE__, __LINE__, (msg), STR(variable)); \
        return (retval); \
    }

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_COMMON_H__ */
