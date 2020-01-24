/*
 * ComPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef COMPORT_HPP_
#define COMPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Com/ComBuffer.hpp>


namespace Fw {

    /// Input Com port description
    /// 

    class InputComPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = ComBuffer::SERIALIZED_SIZE + sizeof(U32) //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, ComBuffer &data, U32 context); //!< port callback definition

        InputComPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(ComBuffer &data, U32 context); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Com port description
    /// 
    
    class OutputComPort : public Fw::OutputPortBase {
      public: 
        OutputComPort(void);
        void init(void);
        void addCallPort(InputComPort* callPort);
        void invoke(ComBuffer &data, U32 context);
      protected:
      private:
        InputComPort* m_port;
    };
} // end namespace Fw
#endif /* COM_HPP_ */

