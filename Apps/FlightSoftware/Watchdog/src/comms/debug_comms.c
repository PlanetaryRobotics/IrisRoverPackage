#include "comms/debug_comms.h"

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef ENABLE_DEBUG_ONLY_CODE
static char PRINT_BUFFER[256] = { 0 };
static LanderComms__State* LC_STATE = NULL;
#endif

void DebugComms__registerLanderComms(LanderComms__State* lcState)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    LC_STATE = lcState;
#endif
}

void DebugComms__stringBufferToLander(void* buffer,
                                      size_t bufferLen)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL || buffer == NULL) {
        return;
    }

    LanderComms__txDataUntilSendOrTimeout(LC_STATE,
                                          (const uint8_t*) buffer,
                                          bufferLen,
                                          300);
#endif
}


void DebugComms__tryStringBufferToLanderNonblocking(void* buffer, size_t bufferLen)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL || buffer == NULL) {
        return;
    }

    LanderComms__txData(LC_STATE,
                        (const uint8_t*) buffer,
                        bufferLen);
#endif
}

void DebugComms__printfToLander(const char* fmt,
                                ...)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL || fmt == NULL) {
        return;
    }

    memset(PRINT_BUFFER, 0, sizeof(PRINT_BUFFER));
    sprintf(PRINT_BUFFER, "DEBUG");
    va_list args;
    va_start(args, fmt);
    vsprintf(PRINT_BUFFER + 5, fmt, args);
    va_end(args);

    DebugComms__stringBufferToLander(PRINT_BUFFER, strlen(PRINT_BUFFER));
#endif
}

void DebugComms__tryPrintfToLanderNonblocking(const char* fmt, ...)
{

#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL || fmt == NULL) {
        return;
    }

    memset(PRINT_BUFFER, 0, sizeof(PRINT_BUFFER));
    sprintf(PRINT_BUFFER, "DEBUG");
    va_list args;
    va_start(args, fmt);
    vsprintf(PRINT_BUFFER + 5, fmt, args);
    va_end(args);

    DebugComms__tryStringBufferToLanderNonblocking(PRINT_BUFFER, strlen(PRINT_BUFFER));
#endif
}

void DebugComms__printDataAsHexToLander(const uint8_t* data,
                                        size_t dataLen,
                                        BOOL withSpaces)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL || data == NULL) {
        return;
    }

    memset(PRINT_BUFFER, 0, sizeof(PRINT_BUFFER));
    int length = 0;
    char fmtString[8] = { 0 };
    strncpy(fmtString, withSpaces ? " %s%x" : "%s%x", 7);
    length += sprintf(PRINT_BUFFER + length, "DEBUG");
    for (size_t i = 0; i < dataLen; ++i) {
        length += sprintf(PRINT_BUFFER + length, fmtString, ((data[i] <= 0xF) ? "0" : ""), data[i]);
    }
    length += sprintf(PRINT_BUFFER + length, "\n");

    DebugComms__stringBufferToLander(PRINT_BUFFER, strlen(PRINT_BUFFER));
#endif
}

void DebugComms__flush(void)
{
#ifdef ENABLE_DEBUG_ONLY_CODE
    if (LC_STATE == NULL) {
        return;
    }

    LanderComms__flushTx(LC_STATE);
#endif
}


