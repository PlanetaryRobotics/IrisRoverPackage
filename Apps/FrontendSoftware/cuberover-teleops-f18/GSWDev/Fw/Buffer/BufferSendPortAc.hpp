/*
 * BufferSendPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef BUFFERSENDPORT_HPP_
#define BUFFERSENDPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Buffer/BufferSerializableAc.hpp>


namespace Fw {

    /// Input BufferSend port description
    /// 

    class InputBufferSendPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = Fw::Buffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::Buffer &fwBuffer); //!< port callback definition

        InputBufferSendPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(Fw::Buffer &fwBuffer); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input BufferSend port description
    /// 
    
    class OutputBufferSendPort : public Fw::OutputPortBase {
      public: 
        OutputBufferSendPort(void);
        void init(void);
        void addCallPort(InputBufferSendPort* callPort);
        void invoke(Fw::Buffer &fwBuffer);
      protected:
      private:
        InputBufferSendPort* m_port;
    };
} // end namespace Fw
#endif /* BUFFERSEND_HPP_ */

