#include <Svc/LinuxTime/CubeRoverTimeImpl.hpp>
#include <Fw/Time/Time.hpp>

#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>


namespace Svc {

    #if FW_OBJECT_NAMES == 1
    CubeRoverTimeImpl::CubeRoverTimeImpl(const char* name) : TimeComponentBase(name)
    #else
    CubeRoverTimeImpl::CubeRoverTimeImpl()
    #endif
    {
    }

    CubeRoverTimeImpl::~CubeRoverTimeImpl() {
    }

    void CubeRoverTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {

        // Assumption here is that configTICK_RATE_HZ is set to 1000
        // from the freeRTOS configuration.
        TickType_t tickCount = xTaskGetTickCount();
        uint32_t second = tickCount / configTICK_RATE_HZ;
        uint32_t ticksRemainder = tickCount - (second * configTICK_RATE_HZ);

        // 1000000 us/sec, and configTICK_RATE_HZ ticks/sec, --> (us/sec) / (ticks/sec) = (us/tick)
        // Then ticksRemainder (in tick units) * (us / tick) = us
        uint32_t micros = ticksRemainder * (1000000 / configTICK_RATE_HZ);
        time.set(TB_WORKSTATION_TIME, 0, second, micros);
    }

    void CubeRoverTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }
}
