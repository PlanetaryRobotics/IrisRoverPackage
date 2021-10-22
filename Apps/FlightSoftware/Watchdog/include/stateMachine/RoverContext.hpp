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
    enum PersistentStatusBitIndices
    {
        I2C_INITIALIZED = 0,
        UART0_INITIALIZED,
        UART1_INITIALIZED,
        THE_NEXT_THING,

    };

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

            HeaterParams m_heaterParams;
            bool m_isDeployed;
    };

}

#endif /* __WATCHDOG_ROVER_CONTEXT_HPP__ */
