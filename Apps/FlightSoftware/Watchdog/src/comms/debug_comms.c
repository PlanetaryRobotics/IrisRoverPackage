#include "comms/debug_comms.h"

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


static char PRINT_BUFFER[256] = { 0 };
static LanderComms__State* LC_STATE = NULL;
static HerculesComms__State* HC_STATE = NULL;

static BOOL DEBUG_COMMS_ENABLED =
#ifdef ENABLE_DEBUG_ONLY_CODE
        TRUE;
#else
        FALSE;
#endif

void DebugComms__setEnabled(BOOL enabled)
{
    DEBUG_COMMS_ENABLED = enabled;
}

void DebugComms__registerLanderComms(LanderComms__State* lcState)
{
    LC_STATE = lcState;
}

void DebugComms__registerHerculesComms(HerculesComms__State* hcState)
{
    HC_STATE = hcState;
}

void DebugComms__stringBufferToLander(void* buffer,
                                      size_t bufferLen)
{
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if (buffer == NULL) {
        return;
    }

    if (LC_STATE != NULL) {
        LanderComms__txDataUntilSendOrTimeout(LC_STATE,
                                              (const uint8_t*) buffer,
                                              bufferLen,
                                              300);
    }

    if (HC_STATE != NULL) {
        HerculesComms__txDownlinkDataUntilSendOrTimeout(HC_STATE,
                                                        (const uint8_t*) buffer,
                                                        bufferLen,
                                                        300);
    }
}


void DebugComms__tryStringBufferToLanderNonblocking(void* buffer, size_t bufferLen)
{
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if (buffer == NULL) {
        return;
    }

    if (LC_STATE != NULL) {
        LanderComms__txData(LC_STATE,
                            (const uint8_t*) buffer,
                            bufferLen);
    }

    if (HC_STATE != NULL) {
            HerculesComms__txDownlinkData(HC_STATE,
                                          (const uint8_t*) buffer,
                                           bufferLen);
    }

}

void DebugComms__printfToLander(const char* fmt, ...)
{
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if ((LC_STATE == NULL && HC_STATE == NULL) || fmt == NULL) {
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

void DebugComms__tryPrintfToLanderNonblocking(const char* fmt, ...)
{
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if ((LC_STATE == NULL && HC_STATE == NULL) || fmt == NULL) {
        return;
    }

    memset(PRINT_BUFFER, 0, sizeof(PRINT_BUFFER));
    sprintf(PRINT_BUFFER, "DEBUG");
    va_list args;
    va_start(args, fmt);
    vsprintf(PRINT_BUFFER + 5, fmt, args);
    va_end(args);

    DebugComms__tryStringBufferToLanderNonblocking(PRINT_BUFFER, strlen(PRINT_BUFFER));
}

void DebugComms__printDataAsHexToLander(const uint8_t* data,
                                        size_t dataLen,
                                        BOOL withSpaces)
{
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if ((LC_STATE == NULL && HC_STATE == NULL) || data == NULL) {
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
    if (!DEBUG_COMMS_ENABLED) {
        return;
    }

    if (LC_STATE != NULL) {
        LanderComms__flushTx(LC_STATE);
    }

    if (HC_STATE != NULL) {
        HerculesComms__flushTx(HC_STATE);
    }
}


