#ifndef __WATCHDOG_DEBUG_COMMS_H__
#define __WATCHDOG_DEBUG_COMMS_H__

#include "comms/lander_comms.h"
#include "common.h"
#include "assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

void DebugComms__registerLanderComms(LanderComms__State* lcState);

void DebugComms__stringBufferToLander(void* buffer, size_t bufferLen);
void DebugComms__tryStringBufferToLanderNonblocking(void* buffer, size_t bufferLen);

void DebugComms__printfToLander(const char* fmt, ...);
void DebugComms__tryPrintfToLanderNonblocking(const char* fmt, ...);

void DebugComms__printDataAsHexToLander(const uint8_t* data, size_t dataLen, BOOL withSpaces);

void DebugComms__flush(void);

#define XSTR(s) STR(s)
#define STR(s) #s

#define DPRINTF(...) DebugComms__printfToLander(__VA_ARGS__)
#define DPRINTF_ERR(...) DebugComms__printfToLander(__VA_ARGS__)
#define DTRACE() DebugComms__printfToLander("[%s:%d]\n", __FILE__, __LINE__)

#define DEBUG_LOG_NULL_CHECK(variable, msg) \
    if (NULL == (variable)) { \
        DPRINTF_ERR("[%s:%d] %s: %s is NULL\n", __FILE__, __LINE__, (msg), STR(variable)); \
    }

#define DEBUG_LOG_CHECK_STATUS(expected, actual, msg) \
    if ((expected) != (actual)) { \
        DPRINTF_ERR("[%s:%d] %s: expected = %d, actual = %d\n", __FILE__, __LINE__, (msg), (expected), (actual)); \
    }

#define DEBUG_LOG_NULL_CHECK_RETURN(variable, msg, retval) \
    if (NULL == (variable)) { \
        DPRINTF_ERR("[%s:%d] %s: %s is NULL\n", __FILE__, __LINE__, (msg), STR(variable)); \
        return retval; \
    }

#define DEBUG_LOG_CHECK_STATUS_RETURN(expected, actual, msg, retval) \
    if ((expected) != (actual)) { \
        DPRINTF_ERR("[%s:%d] %s: expected = %d, actual = %d\n", __FILE__, __LINE__, (msg), STR(variable)); \
        return retval; \
    }

#define DEBUG_ASSERT(expression) \
    if (!(expression)) { \
        DPRINTF_ERR("[%s:%d] %s is not true\n", __FILE__, __LINE__, STR(expression)); \
        DebugComms__flush(); \
        assert(expression); \
    }

#define DEBUG_ASSERT_EQUAL(left, right) \
    if (!((left) == (right))) { \
        DPRINTF_ERR("[%s:%d] %s (%d) == %s (%d) is not true\n", __FILE__, __LINE__, STR(left), (int)(left), STR(right), (int)(right)); \
        DebugComms__flush(); \
        assert((left) == (right)); \
    }

#define DEBUG_ASSERT_NOT_EQUAL(left, right) \
        if (!((left) != (right))) { \
            DPRINTF_ERR("[%s:%d] %s (%d) != %s (%d) is not true\n", __FILE__, __LINE__, STR(left), (int)(left), STR(right), (int)(right)); \
            DebugComms__flush(); \
            assert((left) != (right)); \
        }

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_DEBUG_COMMS_H__ */
