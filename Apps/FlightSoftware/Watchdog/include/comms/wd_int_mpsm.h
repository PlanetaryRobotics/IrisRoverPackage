/*
 * wd_int_mpsm.h
 *
 *  Created on: Aug 17, 2022
 *      Author: iris
 */

#ifndef INCLUDE_COMMS_WD_INT_MPSM_H_
#define INCLUDE_COMMS_WD_INT_MPSM_H_

#include <stdint.h>
#include <stddef.h>
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogWdIntMpsm WD_INT Message Parsing State Machine
 * @addtogroup watchdogWdIntMpsm
 * @{
 */

/**
 * @brief Possible return statuses for WdIntMpsm functions.
 */
typedef enum WdIntMpsm__Status {
    WD_INT_MPSM__STATUS__POWER_CYCLE_RADIO = 6, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__POWER_CYCLE_HERCULES = 5, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__PARSED_EXIT_STASIS = 4, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__PARSED_ENTER_STASIS = 3, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__PARSED_GOT_WIFI = 2, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__NEED_MORE_DATA = 1, /*!< The function was successful, but needs more data to parse a message. */

    WD_INT_MPSM__STATUS__ERROR_INTERNAL_ERROR = -255 /*!< An unexpected error occurred */
} WdIntMpsm__Status;


WdIntMpsm__Status WdIntMpsm__processEdge(BOOL edgeWasRising, uint16_t flatDuration);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* INCLUDE_COMMS_WD_INT_MPSM_H_ */
