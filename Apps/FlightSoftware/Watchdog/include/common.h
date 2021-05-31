#ifndef __WATCHDOG_COMMON_H__
#define __WATCHDOG_COMMON_H__

#include <stdboool.h>

// Some definitions to identify magic numbers in the code
#define BOOL bool
static const BOOL TRUE = true;
static const BOOL FALSE = false;

#define LANDER_COMMS__MAX_RX_SLIP_MESSAGE_SIZE 256
#define HERCULES_COMMS__MAX_RX_DATA_SIZE 256

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

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))


#endif /* __WATCHDOG_COMMON_H__ */