/*
 * CmdResponsePort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef CMDRESPONSEPORT_HPP_
#define CMDRESPONSEPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>


namespace Fw {

    typedef enum {
        COMMAND_OK,  // Command successfully executed
        COMMAND_INVALID_OPCODE,  // Invalid opcode dispatched
        COMMAND_VALIDATION_ERROR,  // Command failed validation
        COMMAND_FORMAT_ERROR,  // Command failed to deserialize
        COMMAND_EXECUTION_ERROR,  // Command had execution error
        COMMAND_BUSY,  // Component busy
        CommandResponse_MAX
    } CommandResponse; //!< CommandResponse enumeration argument

    /// Input CmdResponse port description
    /// 

    class InputCmdResponsePort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwOpcodeType) + sizeof(U32) + sizeof(NATIVE_INT_TYPE) //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwOpcodeType opCode, U32 cmdSeq, CommandResponse response); //!< port callback definition

        InputCmdResponsePort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(FwOpcodeType opCode, U32 cmdSeq, CommandResponse response); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input CmdResponse port description
    /// 
    
    class OutputCmdResponsePort : public Fw::OutputPortBase {
      public: 
        OutputCmdResponsePort(void);
        void init(void);
        void addCallPort(InputCmdResponsePort* callPort);
        void invoke(FwOpcodeType opCode, U32 cmdSeq, CommandResponse response);
      protected:
      private:
        InputCmdResponsePort* m_port;
    };
} // end namespace Fw
#endif /* CMDRESPONSE_HPP_ */

