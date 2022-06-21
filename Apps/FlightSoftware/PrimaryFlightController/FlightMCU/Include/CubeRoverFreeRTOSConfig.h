#include <stdio.h>

#ifndef CUBEROVERFREERTOSCONFIG_H_
#define CUBEROVERFREERTOSCONFIG_H_

// fputs is MUCH faster than fprintf
#define DEBUG(str) {                                \
    fputs(str, stderr);                             \
    fflush(stderr);                                 \
}

/*
#define traceTASK_SWITCHED_OUT() DEBUG("RTOS: Task switch out\n");

#define traceTASK_SWITCHED_IN() DEBUG("RTOS: Task switch in\n");

#define traceTASK_CREATE(xTask)  {                  \
    DEBUG("RTOS: Task created: ");                  \
    DEBUG(xTask->pcTaskName);                       \
    DEBUG("\n");                                    \
}

#define traceTASK_SUSPEND(xTask)  {                 \
    DEBUG("RTOS: Task suspended: ");                 \
    DEBUG(xTask->pcTaskName);                       \
    DEBUG("\n");                                    \
}

#define traceTASK_RESUME(xTask)  {                  \
    DEBUG("RTOS: Task resumed: ");                   \
    DEBUG(xTask->pcTaskName);                       \
    DEBUG("\n");                                    \
}

#define traceTASK_RESUME_FROM_ISR(xTask)  {         \
    DEBUG("RTOS: Task resumed (from ISR): ");       \
    DEBUG(xTask->pcTaskName);                       \
    DEBUG("\n");                                    \
}*/

/*#define traceQUEUE_CREATE(pxNewQueue) {             \
    DEBUG("RTOS: Queue created: head ");            \
    fprintf(stderr, "0x%x\n", pxNewQueue->pcHead);  \
}

#define traceQUEUE_SEND(xQueue) {                   \
    DEBUG("RTOS: Sent to queue ");                  \
    fprintf(stderr, "0x%x\n", xQueue->pcHead);      \
}

#define traceQUEUE_RECEIVE(xQueue) {                \
    DEBUG("RTOS: Queue received ");                 \
    fprintf(stderr, "0x%x\n", xQueue->pcHead);      \
}*/

#endif  // CUBEROVERFREERTOSCONFIG_H_
