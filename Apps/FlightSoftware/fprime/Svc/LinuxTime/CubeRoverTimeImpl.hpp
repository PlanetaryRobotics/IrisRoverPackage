#ifndef CUBEROVERTIMEIMPL_HPP_
#define CUBEROVERTIMEIMPL_HPP_

#include <Svc/Time/TimeComponentAc.hpp>

namespace Svc {

class CubeRoverTimeImpl: public TimeComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        CubeRoverTimeImpl(const char* compName);
#else
        CubeRoverTimeImpl();
#endif
        virtual ~CubeRoverTimeImpl();
        void init(NATIVE_INT_TYPE instance);
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif /* CUBEROVERTIMEIMPL_HPP_ */
