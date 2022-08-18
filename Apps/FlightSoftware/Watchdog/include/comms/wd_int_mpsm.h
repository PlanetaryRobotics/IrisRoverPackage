/*
 * wd_int_mpsm.h
 *
 *  Created on: Aug 17, 2022
 *      Author: iris
 */

#ifndef INCLUDE_COMMS_WD_INT_MPSM_H_
#define INCLUDE_COMMS_WD_INT_MPSM_H_

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
 * @brief An opaque structure that contains the internal state of the WdIntMpsm module.
 */
typedef struct WdIntMpsm__State WdIntMpsm__State;

/**
 * @brief Possible return statuses for WdIntMpsm functions.
 */
typedef enum WdIntMpsm__Status {
    WD_INT_MPSM__STATUS__PARSED_MESSAGE_TYPE_1 = 4, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__PARSED_MESSAGE_TYPE_2 = 3, /*!< The function was successful, and successfully parsed a complete message. */
    WD_INT_MPSM__STATUS__PARSED_MESSAGE_TYPE_3 = 2, /*!< The function was successful, and successfully parsed a complete message. */
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
