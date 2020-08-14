/*
 * SchedPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef SCHEDPORT_HPP_
#define SCHEDPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>


namespace Svc {

    /// Input Sched port description
    /// 

    class InputSchedPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(NATIVE_UINT_TYPE) //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context); //!< port callback definition

        InputSchedPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(NATIVE_UINT_TYPE context); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Sched port description
    /// 
    
    class OutputSchedPort : public Fw::OutputPortBase {
      public: 
        OutputSchedPort(void);
        void init(void);
        void addCallPort(InputSchedPort* callPort);
        void invoke(NATIVE_UINT_TYPE context);
      protected:
      private:
        InputSchedPort* m_port;
    };
} // end namespace Svc
#endif /* SCHED_HPP_ */

