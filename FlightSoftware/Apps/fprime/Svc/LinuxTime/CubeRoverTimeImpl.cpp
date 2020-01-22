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
        uint32_t second = xTaskGetTickCount() / configTICK_RATE_HZ;
        time.set(TB_WORKSTATION_TIME, 0, second, xTaskGetTickCount() * 1000);
    }

    void CubeRoverTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }
}
