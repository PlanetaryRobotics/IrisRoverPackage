/*
 * TlmPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef TLMPORT_HPP_
#define TLMPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Time/Time.hpp>


namespace Fw {

    /// Input Tlm port description
    /// 

    class InputTlmPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + TlmBuffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, TlmBuffer &val); //!< port callback definition

        InputTlmPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(FwChanIdType id, Fw::Time &timeTag, TlmBuffer &val); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Tlm port description
    /// 
    
    class OutputTlmPort : public Fw::OutputPortBase {
      public: 
        OutputTlmPort(void);
        void init(void);
        void addCallPort(InputTlmPort* callPort);
        void invoke(FwChanIdType id, Fw::Time &timeTag, TlmBuffer &val);
      protected:
      private:
        InputTlmPort* m_port;
    };
} // end namespace Fw
#endif /* TLM_HPP_ */

