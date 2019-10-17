#include <Svc/LinuxTime/CubeRoverTimeImpl.hpp>
#include <Fw/Time/Time.hpp>

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
       // timespec stime;
       // (void)clock_gettime(CLOCK_REALTIME,&stime);
       //  time.set(TB_WORKSTATION_TIME,0, stime.tv_sec, stime.tv_nsec/1000);
    }

    void CubeRoverTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }
}
