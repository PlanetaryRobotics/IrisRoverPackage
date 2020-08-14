// ====================================================================== 
// \title  CommandDispatcherComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for CommandDispatcher component base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef COMMANDDISPATCHER_COMP_HPP_
#define COMMANDDISPATCHER_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Os/Mutex.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Cmd/CmdPortAc.hpp>
#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include <Fw/Cmd/CmdRegPortAc.hpp>
#include <Fw/Com/ComPortAc.hpp>
#include <Svc/Ping/PingPortAc.hpp>
#include <Fw/Tlm/TlmPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif

namespace Svc {

  //! \class CommandDispatcherComponentBase
  //! \brief Auto-generated base for CommandDispatcher component
  //!
  class CommandDispatcherComponentBase :
    public Fw::ActiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class CommandDispatcherComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return compCmdReg[portNum]
    //!
    Fw::InputCmdRegPort* get_compCmdReg_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return compCmdStat[portNum]
    //!
    Fw::InputCmdResponsePort* get_compCmdStat_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return seqCmdBuff[portNum]
    //!
    Fw::InputComPort* get_seqCmdBuff_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return pingIn[portNum]
    //!
    Svc::InputPingPort* get_pingIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return CmdDisp[portNum]
    //!
    Fw::InputCmdPort* get_CmdDisp_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to compCmdSend[portNum]
    //!
    void set_compCmdSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdPort *port /*!< The port*/
    );

    //! Connect port to seqCmdStatus[portNum]
    //!
    void set_seqCmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdResponsePort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputPingPort *port /*!< The port*/
    );

    //! Connect port to CmdStatus[portNum]
    //!
    void set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdResponsePort *port /*!< The port*/
    );

    //! Connect port to CmdReg[portNum]
    //!
    void set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdRegPort *port /*!< The port*/
    );

    //! Connect port to Tlm[portNum]
    //!
    void set_Tlm_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTlmPort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTimePort *port /*!< The port*/
    );

    //! Connect port to Log[portNum]
    //!
    void set_Log_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogPort *port /*!< The port*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Connect port to LogText[portNum]
    //!
    void set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogTextPort *port /*!< The port*/
    );
#endif

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to compCmdSend[portNum]
    //!
    void set_compCmdSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to seqCmdStatus[portNum]
    //!
    void set_seqCmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to CmdStatus[portNum]
    //!
    void set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to CmdReg[portNum]
    //!
    void set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to Tlm[portNum]
    //!
    void set_Tlm_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to Log[portNum]
    //!
    void set_Log_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Connect port to LogText[portNum]
    //!
    void set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );
#endif

#endif

  public:

    // ----------------------------------------------------------------------
    // Command registration
    // ----------------------------------------------------------------------

    //! \brief Register commands with the Command Dispatcher
    //!
    //! Connect the dispatcher first
    //!
    void regCommands(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a CommandDispatcherComponentBase object
    //!
    CommandDispatcherComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a CommandDispatcherComponentBase object
    //!
    CommandDispatcherComponentBase(void);
#endif

    //! Initialize a CommandDispatcherComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a CommandDispatcherComponentBase object
    //!
    virtual ~CommandDispatcherComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    //! Mutex operations for guarded ports.
    // ----------------------------------------------------------------------
    //! You can override these operations to provide more sophisticated
    //! synchronization.
    // ----------------------------------------------------------------------

    //! Lock the guarded mutex
    //!
    virtual void lock(void);

    //! Unlock the guarded mutex
    //!
    virtual void unLock(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port compCmdReg
    //
    virtual void compCmdReg_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode /*!< Command Op Code*/
    ) = 0;

    //! Handler for input port compCmdStat
    //
    virtual void compCmdStat_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    ) = 0;

    //! Handler for input port seqCmdBuff
    //
    virtual void seqCmdBuff_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    ) = 0;

    //! Handler for input port pingIn
    //
    virtual void pingIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port compCmdReg
    //!
    void compCmdReg_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode /*!< Command Op Code*/
    );

    //! Handler base-class function for input port compCmdStat
    //!
    void compCmdStat_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Handler base-class function for input port seqCmdBuff
    //!
    void seqCmdBuff_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Handler base-class function for input port pingIn
    //!
    void pingIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port compCmdStat
    //!
    virtual void compCmdStat_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Pre-message hook for async input port seqCmdBuff
    //!
    virtual void seqCmdBuff_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Pre-message hook for async input port pingIn
    //!
    virtual void pingIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port compCmdSend
    //!
    void compCmdSend_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
    );

    //! Invoke output port seqCmdStatus
    //!
    void seqCmdStatus_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Invoke output port pingOut
    //!
    void pingOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of compCmdReg input ports
    //!
    //! \return The number of compCmdReg input ports
    //!
    NATIVE_INT_TYPE getNum_compCmdReg_InputPorts(void);

    //! Get the number of compCmdStat input ports
    //!
    //! \return The number of compCmdStat input ports
    //!
    NATIVE_INT_TYPE getNum_compCmdStat_InputPorts(void);

    //! Get the number of seqCmdBuff input ports
    //!
    //! \return The number of seqCmdBuff input ports
    //!
    NATIVE_INT_TYPE getNum_seqCmdBuff_InputPorts(void);

    //! Get the number of pingIn input ports
    //!
    //! \return The number of pingIn input ports
    //!
    NATIVE_INT_TYPE getNum_pingIn_InputPorts(void);

    //! Get the number of CmdDisp input ports
    //!
    //! \return The number of CmdDisp input ports
    //!
    NATIVE_INT_TYPE getNum_CmdDisp_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_COMPCMDREG_INPUT_PORTS = 30,
       NUM_COMPCMDSTAT_INPUT_PORTS = 1,
       NUM_SEQCMDBUFF_INPUT_PORTS = 5,
       NUM_PINGIN_INPUT_PORTS = 1,
       NUM_CMDDISP_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of compCmdSend output ports
    //!
    //! \return The number of compCmdSend output ports
    //!
    NATIVE_INT_TYPE getNum_compCmdSend_OutputPorts(void);

    //! Get the number of seqCmdStatus output ports
    //!
    //! \return The number of seqCmdStatus output ports
    //!
    NATIVE_INT_TYPE getNum_seqCmdStatus_OutputPorts(void);

    //! Get the number of pingOut output ports
    //!
    //! \return The number of pingOut output ports
    //!
    NATIVE_INT_TYPE getNum_pingOut_OutputPorts(void);

    //! Get the number of CmdStatus output ports
    //!
    //! \return The number of CmdStatus output ports
    //!
    NATIVE_INT_TYPE getNum_CmdStatus_OutputPorts(void);

    //! Get the number of CmdReg output ports
    //!
    //! \return The number of CmdReg output ports
    //!
    NATIVE_INT_TYPE getNum_CmdReg_OutputPorts(void);

    //! Get the number of Tlm output ports
    //!
    //! \return The number of Tlm output ports
    //!
    NATIVE_INT_TYPE getNum_Tlm_OutputPorts(void);

    //! Get the number of Time output ports
    //!
    //! \return The number of Time output ports
    //!
    NATIVE_INT_TYPE getNum_Time_OutputPorts(void);

    //! Get the number of Log output ports
    //!
    //! \return The number of Log output ports
    //!
    NATIVE_INT_TYPE getNum_Log_OutputPorts(void);

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Get the number of LogText output ports
    //!
    //! \return The number of LogText output ports
    //!
    NATIVE_INT_TYPE getNum_LogText_OutputPorts(void);
#endif

  
    enum {
       NUM_COMPCMDSEND_OUTPUT_PORTS = 30,
       NUM_SEQCMDSTATUS_OUTPUT_PORTS = 5,
       NUM_PINGOUT_OUTPUT_PORTS = 1,
       NUM_CMDSTATUS_OUTPUT_PORTS = 1,
       NUM_CMDREG_OUTPUT_PORTS = 1,
       NUM_TLM_OUTPUT_PORTS = 1,
       NUM_TIME_OUTPUT_PORTS = 1,
       NUM_LOG_OUTPUT_PORTS = 1,
       NUM_LOGTEXT_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port compCmdSend is connected
    //!
    //! \return Whether port compCmdSend is connected
    //!
    bool isConnected_compCmdSend_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port seqCmdStatus is connected
    //!
    //! \return Whether port seqCmdStatus is connected
    //!
    bool isConnected_seqCmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port pingOut is connected
    //!
    //! \return Whether port pingOut is connected
    //!
    bool isConnected_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port CmdStatus is connected
    //!
    //! \return Whether port CmdStatus is connected
    //!
    bool isConnected_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port CmdReg is connected
    //!
    //! \return Whether port CmdReg is connected
    //!
    bool isConnected_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port Tlm is connected
    //!
    //! \return Whether port Tlm is connected
    //!
    bool isConnected_Tlm_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port Time is connected
    //!
    //! \return Whether port Time is connected
    //!
    bool isConnected_Time_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port Log is connected
    //!
    //! \return Whether port Log is connected
    //!
    bool isConnected_Log_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Check whether port LogText is connected
    //!
    //! \return Whether port LogText is connected
    //!
    bool isConnected_LogText_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );
#endif

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command opcodes
    // ----------------------------------------------------------------------

    enum {
      OPCODE_CMD_NO_OP = 0, /* No-op command */
      OPCODE_CMD_NO_OP_STRING = 1, /* No-op string command */
      OPCODE_CMD_TEST_CMD_1 = 2, /* No-op command */
      OPCODE_CMD_CLEAR_TRACKING = 3, /* Clear command tracking info to recover from components not returning status */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handlers to implement
    // ----------------------------------------------------------------------

    //! Handler for command CMD_NO_OP
    /* No-op command */
    virtual void CMD_NO_OP_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

    //! Handler for command CMD_NO_OP_STRING
    /* No-op string command */
    virtual void CMD_NO_OP_STRING_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        const Fw::CmdStringArg& arg1 /*!< The String command argument*/
    ) = 0;

    //! Handler for command CMD_TEST_CMD_1
    /* No-op command */
    virtual void CMD_TEST_CMD_1_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        I32 arg1, /*!< The I32 command argument*/
        F32 arg2, /*!< The F32 command argument*/
        U8 arg3 /*!< The U8 command argument*/
    ) = 0;

    //! Handler for command CMD_CLEAR_TRACKING
    /* Clear command tracking info to recover from components not returning status */
    virtual void CMD_CLEAR_TRACKING_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for async commands.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing the
    // corresponding command. By default they do nothing. You can
    // override them to provide specific pre-command behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for command CMD_NO_OP
    //!
    virtual void CMD_NO_OP_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CMD_NO_OP_STRING
    //!
    virtual void CMD_NO_OP_STRING_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CMD_TEST_CMD_1
    //!
    virtual void CMD_TEST_CMD_1_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CMD_CLEAR_TRACKING
    //!
    virtual void CMD_CLEAR_TRACKING_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handler base-class functions.
    // Call these functions directly to bypass the command input port.
    // ----------------------------------------------------------------------

    //! Base-class handler function for command CMD_NO_OP
    //! 
    void CMD_NO_OP_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CMD_NO_OP_STRING
    //! 
    void CMD_NO_OP_STRING_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CMD_TEST_CMD_1
    //! 
    void CMD_TEST_CMD_1_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CMD_CLEAR_TRACKING
    //! 
    void CMD_CLEAR_TRACKING_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command response
    // ----------------------------------------------------------------------

    //! Emit command response
    //!
    void cmdResponse_out(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CommandResponse response /*!< The command response*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event IDs
    // ----------------------------------------------------------------------

    enum {
      EVENTID_OPCODEREGISTERED = 0, /* Op code registered event */
      EVENTID_OPCODEDISPATCHED = 1, /* Op code dispatched event */
      EVENTID_OPCODECOMPLETED = 2, /* Op code completed event */
      EVENTID_OPCODEERROR = 3, /* Op code completed with error event */
      EVENTID_MALFORMEDCOMMAND = 4, /* Received a malformed command packet */
      EVENTID_INVALIDCOMMAND = 5, /* Received an invalid opcode */
      EVENTID_TOOMANYCOMMANDS = 6, /* Exceeded the number of commands that can be simultaneously executed */
      EVENTID_NOOPRECEIVED = 7, /* The command dispatcher has successfully recieved a NO-OP command */
      EVENTID_NOOPSTRINGRECEIVED = 8, /* The command dispatcher has successfully recieved a NO-OP command from GUI with a string */
      EVENTID_TESTCMD1ARGS = 9, /* This log event message returns the TEST_CMD_1 arguments. */
    };
    
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event enums
    // ----------------------------------------------------------------------

    typedef enum {
      ERR_INVALID_OPCODE, /*  //<! Invalid opcode dispatched */
      ERR_VALIDATION_ERROR, /*  //<! Command failed validation */
      ERR_FORMAT_ERROR, /*  //<! Command failed to deserialize */
      ERR_EXECUTION_ERROR, /*  //<! Command had execution error */
      ERR_BUSY, /*  //<! Component busy */
      ERR_UNEXP, /*  //<! Unexpected response */
      ErrorResponse_MAX
    } ErrorResponse;

    typedef enum {
      ERR_BUFFER_TOO_SMALL, /*  //<! Buffer too small */
      ERR_BUFFER_FORMAT, /*  //<! Buffer wrong format */
      ERR_SIZE_MISMATCH, /*  //<! Buffer size mismatch */
      ERR_TYPE_MISMATCH, /*  //<! Buffer type mismatch */
      ERR_UNEXP_STAT, /*  //<! Unexpected status */
      CmdSerError_MAX
    } CmdSerError;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event OpCodeRegistered
    //!
    /* Op code registered event */
    void log_DIAGNOSTIC_OpCodeRegistered(
        U32 Opcode, /*!< The opcode to register*/
        I32 port, /*!< The registration port*/
        I32 slot /*!< The dispatch slot it was placed in*/
    );
    

    //! Log event OpCodeDispatched
    //!
    /* Op code dispatched event */
    void log_COMMAND_OpCodeDispatched(
        U32 Opcode, /*!< The opcode dispatched*/
        I32 port /*!< The port dispatched to*/
    );
    

    //! Log event OpCodeCompleted
    //!
    /* Op code completed event */
    void log_COMMAND_OpCodeCompleted(
        U32 Opcode /*!< The I32 command argument*/
    );
    

    //! Log event OpCodeError
    //!
    /* Op code completed with error event */
    void log_WARNING_HI_OpCodeError(
        U32 Opcode, /*!< The opcode with the error*/
        ErrorResponse error /*!< The error value*/
    );
    

    //! Log event MalformedCommand
    //!
    /* Received a malformed command packet */
    void log_WARNING_HI_MalformedCommand(
        CmdSerError Status /*!< The deserialization error*/
    );
    

    //! Log event InvalidCommand
    //!
    /* Received an invalid opcode */
    void log_WARNING_HI_InvalidCommand(
        U32 Opcode /*!< Invalid opcode*/
    );
    

    //! Log event TooManyCommands
    //!
    /* Exceeded the number of commands that can be simultaneously executed */
    void log_WARNING_HI_TooManyCommands(
        U32 Opcode /*!< The opcode that overflowed the list*/
    );
    

    //! Log event NoOpReceived
    //!
    /* The command dispatcher has successfully recieved a NO-OP command */
    void log_ACTIVITY_HI_NoOpReceived(
        void
    );
    

    //! Log event NoOpStringReceived
    //!
    /* The command dispatcher has successfully recieved a NO-OP command from GUI with a string */
    void log_ACTIVITY_HI_NoOpStringReceived(
        Fw::LogStringArg& message /*!< The NO-OP string that is generated*/
    );
    

    //! Log event TestCmd1Args
    //!
    /* This log event message returns the TEST_CMD_1 arguments. */
    void log_ACTIVITY_HI_TestCmd1Args(
        I32 arg1, /*!< Arg1*/
        F32 arg2, /*!< Arg2*/
        U8 arg3 /*!< Arg3*/
    );
    

  PROTECTED:

    // ----------------------------------------------------------------------
    // Channel IDs
    // ----------------------------------------------------------------------

    enum {
      CHANNELID_COMMANDSDISPATCHED = 0, //!< Channel ID for CommandsDispatched
      CHANNELID_COMMANDERRORS = 1, //!< Channel ID for CommandErrors
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Telemetry write functions
    // ----------------------------------------------------------------------

    //! Write telemetry channel CommandsDispatched
    //!
    /* Number of commands dispatched */
    void tlmWrite_CommandsDispatched(
        U32 arg /*!< The telemetry value*/
    );

    //! Write telemetry channel CommandErrors
    //!
    /* Number of command errors */
    void tlmWrite_CommandErrors(
        U32 arg /*!< The telemetry value*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Time
    // ----------------------------------------------------------------------

    //! Get the time
    //!
    //! \return The current time
    //!
    Fw::Time getTime(void);


  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed input ports
    // ----------------------------------------------------------------------

    //! Input port compCmdReg
    //!
    Fw::InputCmdRegPort m_compCmdReg_InputPort[NUM_COMPCMDREG_INPUT_PORTS];

    //! Input port compCmdStat
    //!
    Fw::InputCmdResponsePort m_compCmdStat_InputPort[NUM_COMPCMDSTAT_INPUT_PORTS];

    //! Input port seqCmdBuff
    //!
    Fw::InputComPort m_seqCmdBuff_InputPort[NUM_SEQCMDBUFF_INPUT_PORTS];

    //! Input port pingIn
    //!
    Svc::InputPingPort m_pingIn_InputPort[NUM_PINGIN_INPUT_PORTS];

    //! Input port CmdDisp
    //!
    Fw::InputCmdPort m_CmdDisp_InputPort[NUM_CMDDISP_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port compCmdSend
    //!
    Fw::OutputCmdPort m_compCmdSend_OutputPort[NUM_COMPCMDSEND_OUTPUT_PORTS];

    //! Output port seqCmdStatus
    //!
    Fw::OutputCmdResponsePort m_seqCmdStatus_OutputPort[NUM_SEQCMDSTATUS_OUTPUT_PORTS];

    //! Output port pingOut
    //!
    Svc::OutputPingPort m_pingOut_OutputPort[NUM_PINGOUT_OUTPUT_PORTS];

    //! Output port CmdStatus
    //!
    Fw::OutputCmdResponsePort m_CmdStatus_OutputPort[NUM_CMDSTATUS_OUTPUT_PORTS];

    //! Output port CmdReg
    //!
    Fw::OutputCmdRegPort m_CmdReg_OutputPort[NUM_CMDREG_OUTPUT_PORTS];

    //! Output port Tlm
    //!
    Fw::OutputTlmPort m_Tlm_OutputPort[NUM_TLM_OUTPUT_PORTS];

    //! Output port Time
    //!
    Fw::OutputTimePort m_Time_OutputPort[NUM_TIME_OUTPUT_PORTS];

    //! Output port Log
    //!
    Fw::OutputLogPort m_Log_OutputPort[NUM_LOG_OUTPUT_PORTS];

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Output port LogText
    //!
    Fw::OutputLogTextPort m_LogText_OutputPort[NUM_LOGTEXT_OUTPUT_PORTS];
#endif

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port compCmdReg
    //!
    static void m_p_compCmdReg_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode /*!< Command Op Code*/
    );

    //! Callback for port compCmdStat
    //!
    static void m_p_compCmdStat_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Callback for port seqCmdBuff
    //!
    static void m_p_seqCmdBuff_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Callback for port pingIn
    //!
    static void m_p_pingIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Callback for port CmdDisp
    //!
    static void m_p_CmdDisp_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
    );

  PRIVATE:

    // ----------------------------------------------------------------------
    // Message dispatch functions
    // ----------------------------------------------------------------------

    //! Called in the message loop to dispatch a message from the queue
    //!
    virtual MsgDispatchStatus doDispatch(void);

  PRIVATE:

    // ----------------------------------------------------------------------
    // Mutexes
    // ----------------------------------------------------------------------

    //! Mutex for guarded ports
    //!
    Os::Mutex m_guardedPortMutex;

  PRIVATE:

    // ----------------------------------------------------------------------
    // First update flags for telemetry channels
    // ----------------------------------------------------------------------

    //! Initialized to true; cleared when channel CommandsDispatched is first updated
    //!
    bool m_first_update_CommandsDispatched;

    //! Initialized to true; cleared when channel CommandErrors is first updated
    //!
    bool m_first_update_CommandErrors;

  PRIVATE:

    // ----------------------------------------------------------------------
    // Last value storage for telemetry channels
    // ----------------------------------------------------------------------

    //! Records the last emitted value for channel CommandsDispatched
    //!
    U32 m_last_CommandsDispatched;

    //! Records the last emitted value for channel CommandErrors
    //!
    U32 m_last_CommandErrors;


  PRIVATE:
    // ----------------------------------------------------------------------
    // Counter values for event throttling
    // ----------------------------------------------------------------------

  };  

} // end namespace Svc
#endif
