#ifndef __WATCHDOG_ROVER_CONTEXT_HPP__
#define __WATCHDOG_ROVER_CONTEXT_HPP__

#include "comms/i2c_sensors.h"
#include "comms/hercules_comms.h"
#include "comms/lander_comms.h"
#include "drivers/adc.h"
#include "flags.h"

#include <stdint.h>
#include <stdbool.h>

namespace iris
{
    class RoverContext
    {
        public:
            UART__Config m_uartConfig;
            UART__State* m_uart0State;
            UART__State* m_uart1State;

            volatile uint16_t m_watchdogFlags;
            uint8_t m_watchdogOpts;

            I2C_Sensors__Readings m_i2cReadings;
            uint8_t m_keepAliveTickCount;
            AdcValues m_adcValues;

            HerculesComms__State* m_hcState;
            LanderComms__State* m_lcState;

            bool m_i2cActive;
            uint16_t m_queuedI2cActions;
            uint8_t m_queuedIOWritePort0Value;
            uint8_t m_queuedIOWritePort1Value;

            bool m_isDeployed;

            // Note: despite the name, not actually persistent. Only the two bools below are actually persistent.
            PersistentState* m_persistantStatePtr;

            bool* m_persistentInMission;
            bool* m_persistentDeployed;
    };

}

#endif /* __WATCHDOG_ROVER_CONTEXT_HPP__ */
