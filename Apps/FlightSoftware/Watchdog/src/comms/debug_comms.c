#include "comms/debug_comms.h"

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static char PRINT_BUFFER[256] = { 0 };
static LanderComms__State* LC_STATE = NULL;

void DebugComms__registerLanderComms(LanderComms__State* lcState)
{
    LC_STATE = lcState;
}

void DebugComms__stringBufferToLander(void* buffer,
                                      size_t bufferLen)
{
    if (LC_STATE == NULL || buffer == NULL) {
        return;
    }

    LanderComms__txDataUntilSendOrTimeout(LC_STATE,
                                          (const uint8_t*) buffer,
                                          bufferLen,
                                          300);
}

void DebugComms__printfToLander(const char* fmt,
                                ...)
{
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
}

void DebugComms__printDataAsHexToLander(const uint8_t* data,
                                        size_t dataLen,
                                        BOOL withSpaces)
{
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
}

void DebugComms__flush(void)
{
    if (LC_STATE == NULL) {
        return;
    }

    LanderComms__flushTx(LC_STATE);
}


