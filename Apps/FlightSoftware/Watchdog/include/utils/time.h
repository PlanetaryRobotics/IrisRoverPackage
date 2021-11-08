
#ifndef __WATCHDOG_TIME_H__
#define __WATCHDOG_TIME_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @defgroup watchdogTime Time
 * @addtogroup watchdogTime
 * @{
 */

void Time__resetToZero(void);

volatile uint16_t* Time__getPointerToCentisecondCount(void);

uint16_t Time__getTimeInCentiseconds(void);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_TIME_H__ */
