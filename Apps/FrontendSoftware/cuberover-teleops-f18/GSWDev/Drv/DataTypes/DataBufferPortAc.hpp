/*
 * DataBufferPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef DATABUFFERPORT_HPP_
#define DATABUFFERPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Drv/DataTypes/DataBuffer.hpp>


namespace Drv {

    /// Input DataBuffer port description
    /// 

    class InputDataBufferPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = Drv::DataBuffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Drv::DataBuffer &buff); //!< port callback definition

        InputDataBufferPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(Drv::DataBuffer &buff); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input DataBuffer port description
    /// 
    
    class OutputDataBufferPort : public Fw::OutputPortBase {
      public: 
        OutputDataBufferPort(void);
        void init(void);
        void addCallPort(InputDataBufferPort* callPort);
        void invoke(Drv::DataBuffer &buff);
      protected:
      private:
        InputDataBufferPort* m_port;
    };
} // end namespace Drv
#endif /* DATABUFFER_HPP_ */

