/*
 * CmdSeqInPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef CMDSEQINPORT_HPP_
#define CMDSEQINPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Types/EightyCharString.hpp>


namespace Svc {

    /// Input CmdSeqIn port description
    /// 

    class InputCmdSeqInPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = Fw::EightyCharString::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::EightyCharString &filename); //!< port callback definition

        InputCmdSeqInPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(Fw::EightyCharString &filename); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input CmdSeqIn port description
    /// 
    
    class OutputCmdSeqInPort : public Fw::OutputPortBase {
      public: 
        OutputCmdSeqInPort(void);
        void init(void);
        void addCallPort(InputCmdSeqInPort* callPort);
        void invoke(Fw::EightyCharString &filename);
      protected:
      private:
        InputCmdSeqInPort* m_port;
    };
} // end namespace Svc
#endif /* CMDSEQIN_HPP_ */

