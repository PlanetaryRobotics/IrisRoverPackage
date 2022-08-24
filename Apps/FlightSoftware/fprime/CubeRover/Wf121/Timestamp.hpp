/**
 * @file Timestamp.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Simple timestamping function used for getting universal time anywhere
 *        it's needed in Wf121 code.
 *
 * This time is referenced based on FreeRTOS scheduler ticks (1ms) since FPrime
 * time is not available in independent FreeRTOS Tasks.
 *
 * NOTE: if `configTICK_RATE_HZ` is changed from `1000`, this may no longer be
 * exactly accurate.
 *
 * @date 2022-08-17
 */

#ifndef CUBEROVER_WF121_WF121_TIMESTAMP_HPP_
#define CUBEROVER_WF121_WF121_TIMESTAMP_HPP_

#include "FreeRTOS.h"
#include "os_task.h"

namespace Wf121
{
    namespace Timestamp
    {
        static const uint32_t MS_PER_TICK = 1000UL / configTICK_RATE_HZ;

        inline uint32_t getTimeMs()
        {
            return xTaskGetTickCount() * MS_PER_TICK;
        }
    }
}

#endif /* CUBEROVER_WF121_WF121_TIMESTAMP_HPP_ */