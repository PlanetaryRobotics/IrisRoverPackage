#include <stdio.h>

#ifndef CUBEROVERFREERTOSCONFIG_H_
#define CUBEROVERFREERTOSCONFIG_H_

#define traceTASK_SWITCHED_OUT() {  \
    fputs("DBG: Task switch\n", stderr);          \
    fflush(stderr);                 \
}

#endif  // CUBEROVERFREERTOSCONFIG_H_
