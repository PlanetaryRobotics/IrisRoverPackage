#ifndef __WATCHDOG_ROVER_CONTEXT_HPP__
#define __WATCHDOG_ROVER_CONTEXT_HPP__

#include "comms/i2c_sensors.h"
#include "comms/hercules_comms.h"
#include "comms/lander_comms.h"
#include "drivers/adc.h"

#include <stdint.h>

namespace iris
{
    class RoverContext
    {
        public:
            volatile uint16_t m_watchdogFlags;
            uint8_t m_watchdogOpts;

            I2C_Sensors__Readings m_i2cReadings;
            uint8_t m_keepAliveTickCount;
            AdcValues m_adcValues;

            HerculesComms__State* m_hcState;
            LanderComms__State* m_lcState;
    };

}

#endif /* __WATCHDOG_ROVER_CONTEXT_HPP__ */
