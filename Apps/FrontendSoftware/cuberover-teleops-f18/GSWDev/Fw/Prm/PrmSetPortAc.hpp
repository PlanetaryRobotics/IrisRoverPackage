/*
 * PrmSetPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef PRMSETPORT_HPP_
#define PRMSETPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Prm/PrmBuffer.hpp>


namespace Fw {

    /// Input PrmSet port description
    /// 

    class InputPrmSetPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwPrmIdType) + ParamBuffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwPrmIdType id, ParamBuffer &val); //!< port callback definition

        InputPrmSetPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(FwPrmIdType id, ParamBuffer &val); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input PrmSet port description
    /// 
    
    class OutputPrmSetPort : public Fw::OutputPortBase {
      public: 
        OutputPrmSetPort(void);
        void init(void);
        void addCallPort(InputPrmSetPort* callPort);
        void invoke(FwPrmIdType id, ParamBuffer &val);
      protected:
      private:
        InputPrmSetPort* m_port;
    };
} // end namespace Fw
#endif /* PRMSET_HPP_ */

