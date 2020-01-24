/*
 * PrmGetPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef PRMGETPORT_HPP_
#define PRMGETPORT_HPP_

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

    typedef enum {
        PARAM_UNINIT,
        PARAM_VALID,
        PARAM_INVALID,
        PARAM_DEFAULT,
        ParamValid_MAX
    } ParamValid; //!< ParamValid enumeration argument

    /// Input PrmGet port description
    /// 

    class InputPrmGetPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwPrmIdType) + ParamBuffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef ParamValid (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwPrmIdType id, ParamBuffer &val); //!< port callback definition

        InputPrmGetPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        ParamValid invoke(FwPrmIdType id, ParamBuffer &val); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input PrmGet port description
    /// 
    
    class OutputPrmGetPort : public Fw::OutputPortBase {
      public: 
        OutputPrmGetPort(void);
        void init(void);
        void addCallPort(InputPrmGetPort* callPort);
        ParamValid invoke(FwPrmIdType id, ParamBuffer &val);
      protected:
      private:
        InputPrmGetPort* m_port;
    };
} // end namespace Fw
#endif /* PRMGET_HPP_ */

