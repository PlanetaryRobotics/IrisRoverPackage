/*
 * CyclePort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef CYCLEPORT_HPP_
#define CYCLEPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Svc/Cycle/TimerVal.hpp>


namespace Svc {

    /// Input Cycle port description
    /// 

    class InputCyclePort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = Svc::TimerVal::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Svc::TimerVal &cycleStart); //!< port callback definition

        InputCyclePort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(Svc::TimerVal &cycleStart); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Cycle port description
    /// 
    
    class OutputCyclePort : public Fw::OutputPortBase {
      public: 
        OutputCyclePort(void);
        void init(void);
        void addCallPort(InputCyclePort* callPort);
        void invoke(Svc::TimerVal &cycleStart);
      protected:
      private:
        InputCyclePort* m_port;
    };
} // end namespace Svc
#endif /* CYCLE_HPP_ */

