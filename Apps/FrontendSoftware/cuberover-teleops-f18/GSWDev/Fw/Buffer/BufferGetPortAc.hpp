/*
 * BufferGetPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef BUFFERGETPORT_HPP_
#define BUFFERGETPORT_HPP_

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

    /// Input BufferGet port description
    /// 

    class InputBufferGetPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(U32) //!< serialized size of port arguments
        };
        typedef Fw::Buffer (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, U32 size); //!< port callback definition

        InputBufferGetPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        Fw::Buffer invoke(U32 size); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input BufferGet port description
    /// 
    
    class OutputBufferGetPort : public Fw::OutputPortBase {
      public: 
        OutputBufferGetPort(void);
        void init(void);
        void addCallPort(InputBufferGetPort* callPort);
        Fw::Buffer invoke(U32 size);
      protected:
      private:
        InputBufferGetPort* m_port;
    };
} // end namespace Fw
#endif /* BUFFERGET_HPP_ */

