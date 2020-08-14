// ====================================================================== 
// \title  CmdSequencerComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for CmdSequencer component base class
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

#ifndef CMDSEQUENCER_COMP_HPP_
#define CMDSEQUENCER_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Cmd/CmdRegPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif
#include <Svc/Ping/PingPortAc.hpp>
#include <Fw/Tlm/TlmPortAc.hpp>
#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Com/ComPortAc.hpp>
#include <Fw/Cmd/CmdPortAc.hpp>
#include <Svc/Sched/SchedPortAc.hpp>
#include <Svc/Seq/CmdSeqInPortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>

namespace Svc {

  //! \class CmdSequencerComponentBase
  //! \brief Auto-generated base for CmdSequencer component
  //!
  class CmdSequencerComponentBase :
    public Fw::ActiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class CmdSequencerComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return pingIn[portNum]
    //!
    Svc::InputPingPort* get_pingIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return cmdResponseIn[portNum]
    //!
    Fw::InputCmdResponsePort* get_cmdResponseIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return cmdIn[portNum]
    //!
    Fw::InputCmdPort* get_cmdIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return schedIn[portNum]
    //!
    Svc::InputSchedPort* get_schedIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return seqRunIn[portNum]
    //!
    Svc::InputCmdSeqInPort* get_seqRunIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to cmdRegOut[portNum]
    //!
    void set_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdRegPort *port /*!< The port*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Connect port to LogText[portNum]
    //!
    void set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogTextPort *port /*!< The port*/
    );
#endif

    //! Connect port to tlmOut[portNum]
    //!
    void set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTlmPort *port /*!< The port*/
    );

    //! Connect port to cmdResponseOut[portNum]
    //!
    void set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdResponsePort *port /*!< The port*/
    );

    //! Connect port to timeCaller[portNum]
    //!
    void set_timeCaller_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTimePort *port /*!< The port*/
    );

    //! Connect port to comCmdOut[portNum]
    //!
    void set_comCmdOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputComPort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputPingPort *port /*!< The port*/
    );

    //! Connect port to logOut[portNum]
    //!
    void set_logOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogPort *port /*!< The port*/
    );

    //! Connect port to seqDone[portNum]
    //!
    void set_seqDone_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdResponsePort *port /*!< The port*/
    );

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to cmdRegOut[portNum]
    //!
    void set_cmdRegOut_OutputPort(
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

    //! Connect port to tlmOut[portNum]
    //!
    void set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to cmdResponseOut[portNum]
    //!
    void set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to timeCaller[portNum]
    //!
    void set_timeCaller_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to comCmdOut[portNum]
    //!
    void set_comCmdOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to logOut[portNum]
    //!
    void set_logOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to seqDone[portNum]
    //!
    void set_seqDone_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

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
    //! Construct a CmdSequencerComponentBase object
    //!
    CmdSequencerComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a CmdSequencerComponentBase object
    //!
    CmdSequencerComponentBase(void);
#endif

    //! Initialize a CmdSequencerComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a CmdSequencerComponentBase object
    //!
    virtual ~CmdSequencerComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port pingIn
    //
    virtual void pingIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    ) = 0;

    //! Handler for input port cmdResponseIn
    //
    virtual void cmdResponseIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    ) = 0;

    //! Handler for input port schedIn
    //
    virtual void schedIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    ) = 0;

    //! Handler for input port seqRunIn
    //
    virtual void seqRunIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::EightyCharString &filename /*!< The sequence file*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port pingIn
    //!
    void pingIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Handler base-class function for input port cmdResponseIn
    //!
    void cmdResponseIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Handler base-class function for input port schedIn
    //!
    void schedIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Handler base-class function for input port seqRunIn
    //!
    void seqRunIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::EightyCharString &filename /*!< The sequence file*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port pingIn
    //!
    virtual void pingIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Pre-message hook for async input port cmdResponseIn
    //!
    virtual void cmdResponseIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Pre-message hook for async input port schedIn
    //!
    virtual void schedIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Pre-message hook for async input port seqRunIn
    //!
    virtual void seqRunIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::EightyCharString &filename /*!< The sequence file*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port comCmdOut
    //!
    void comCmdOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Invoke output port pingOut
    //!
    void pingOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Invoke output port seqDone
    //!
    void seqDone_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of pingIn input ports
    //!
    //! \return The number of pingIn input ports
    //!
    NATIVE_INT_TYPE getNum_pingIn_InputPorts(void);

    //! Get the number of cmdResponseIn input ports
    //!
    //! \return The number of cmdResponseIn input ports
    //!
    NATIVE_INT_TYPE getNum_cmdResponseIn_InputPorts(void);

    //! Get the number of cmdIn input ports
    //!
    //! \return The number of cmdIn input ports
    //!
    NATIVE_INT_TYPE getNum_cmdIn_InputPorts(void);

    //! Get the number of schedIn input ports
    //!
    //! \return The number of schedIn input ports
    //!
    NATIVE_INT_TYPE getNum_schedIn_InputPorts(void);

    //! Get the number of seqRunIn input ports
    //!
    //! \return The number of seqRunIn input ports
    //!
    NATIVE_INT_TYPE getNum_seqRunIn_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_PINGIN_INPUT_PORTS = 1,
       NUM_CMDRESPONSEIN_INPUT_PORTS = 1,
       NUM_CMDIN_INPUT_PORTS = 1,
       NUM_SCHEDIN_INPUT_PORTS = 1,
       NUM_SEQRUNIN_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of cmdRegOut output ports
    //!
    //! \return The number of cmdRegOut output ports
    //!
    NATIVE_INT_TYPE getNum_cmdRegOut_OutputPorts(void);

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Get the number of LogText output ports
    //!
    //! \return The number of LogText output ports
    //!
    NATIVE_INT_TYPE getNum_LogText_OutputPorts(void);
#endif

    //! Get the number of tlmOut output ports
    //!
    //! \return The number of tlmOut output ports
    //!
    NATIVE_INT_TYPE getNum_tlmOut_OutputPorts(void);

    //! Get the number of cmdResponseOut output ports
    //!
    //! \return The number of cmdResponseOut output ports
    //!
    NATIVE_INT_TYPE getNum_cmdResponseOut_OutputPorts(void);

    //! Get the number of timeCaller output ports
    //!
    //! \return The number of timeCaller output ports
    //!
    NATIVE_INT_TYPE getNum_timeCaller_OutputPorts(void);

    //! Get the number of comCmdOut output ports
    //!
    //! \return The number of comCmdOut output ports
    //!
    NATIVE_INT_TYPE getNum_comCmdOut_OutputPorts(void);

    //! Get the number of pingOut output ports
    //!
    //! \return The number of pingOut output ports
    //!
    NATIVE_INT_TYPE getNum_pingOut_OutputPorts(void);

    //! Get the number of logOut output ports
    //!
    //! \return The number of logOut output ports
    //!
    NATIVE_INT_TYPE getNum_logOut_OutputPorts(void);

    //! Get the number of seqDone output ports
    //!
    //! \return The number of seqDone output ports
    //!
    NATIVE_INT_TYPE getNum_seqDone_OutputPorts(void);

  
    enum {
       NUM_CMDREGOUT_OUTPUT_PORTS = 1,
       NUM_LOGTEXT_OUTPUT_PORTS = 1,
       NUM_TLMOUT_OUTPUT_PORTS = 1,
       NUM_CMDRESPONSEOUT_OUTPUT_PORTS = 1,
       NUM_TIMECALLER_OUTPUT_PORTS = 1,
       NUM_COMCMDOUT_OUTPUT_PORTS = 1,
       NUM_PINGOUT_OUTPUT_PORTS = 1,
       NUM_LOGOUT_OUTPUT_PORTS = 1,
       NUM_SEQDONE_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port cmdRegOut is connected
    //!
    //! \return Whether port cmdRegOut is connected
    //!
    bool isConnected_cmdRegOut_OutputPort(
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

    //! Check whether port tlmOut is connected
    //!
    //! \return Whether port tlmOut is connected
    //!
    bool isConnected_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port cmdResponseOut is connected
    //!
    //! \return Whether port cmdResponseOut is connected
    //!
    bool isConnected_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port timeCaller is connected
    //!
    //! \return Whether port timeCaller is connected
    //!
    bool isConnected_timeCaller_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port comCmdOut is connected
    //!
    //! \return Whether port comCmdOut is connected
    //!
    bool isConnected_comCmdOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port pingOut is connected
    //!
    //! \return Whether port pingOut is connected
    //!
    bool isConnected_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port logOut is connected
    //!
    //! \return Whether port logOut is connected
    //!
    bool isConnected_logOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port seqDone is connected
    //!
    //! \return Whether port seqDone is connected
    //!
    bool isConnected_seqDone_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command opcodes
    // ----------------------------------------------------------------------

    enum {
      OPCODE_CS_RUN = 0, /* Run a command sequence file */
      OPCODE_CS_VALIDATE = 1, /* Validate a command sequence file */
      OPCODE_CS_CANCEL = 2, /* Cancel a command sequence */
      OPCODE_CS_START = 3, /* Start running a command sequence */
      OPCODE_CS_STEP = 4, /* Perform one step in a command sequence. Valid only if CmdSequencer is in MANUAL run mode. */
      OPCODE_CS_AUTO = 5, /* Set the run mode to AUTO. */
      OPCODE_CS_MANUAL = 6, /* Set the run mode to MANUAL. */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handlers to implement
    // ----------------------------------------------------------------------

    //! Handler for command CS_Run
    /* Run a command sequence file */
    virtual void CS_Run_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        const Fw::CmdStringArg& fileName /*!< The name of the sequence file*/
    ) = 0;

    //! Handler for command CS_Validate
    /* Validate a command sequence file */
    virtual void CS_Validate_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        const Fw::CmdStringArg& fileName /*!< The name of the sequence file*/
    ) = 0;

    //! Handler for command CS_Cancel
    /* Cancel a command sequence */
    virtual void CS_Cancel_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

    //! Handler for command CS_Start
    /* Start running a command sequence */
    virtual void CS_Start_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

    //! Handler for command CS_Step
    /* Perform one step in a command sequence. Valid only if CmdSequencer is in MANUAL run mode. */
    virtual void CS_Step_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

    //! Handler for command CS_Auto
    /* Set the run mode to AUTO. */
    virtual void CS_Auto_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

    //! Handler for command CS_Manual
    /* Set the run mode to MANUAL. */
    virtual void CS_Manual_cmdHandler(
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

    //! Pre-message hook for command CS_Run
    //!
    virtual void CS_Run_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Validate
    //!
    virtual void CS_Validate_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Cancel
    //!
    virtual void CS_Cancel_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Start
    //!
    virtual void CS_Start_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Step
    //!
    virtual void CS_Step_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Auto
    //!
    virtual void CS_Auto_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command CS_Manual
    //!
    virtual void CS_Manual_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handler base-class functions.
    // Call these functions directly to bypass the command input port.
    // ----------------------------------------------------------------------

    //! Base-class handler function for command CS_Run
    //! 
    void CS_Run_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Validate
    //! 
    void CS_Validate_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Cancel
    //! 
    void CS_Cancel_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Start
    //! 
    void CS_Start_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Step
    //! 
    void CS_Step_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Auto
    //! 
    void CS_Auto_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command CS_Manual
    //! 
    void CS_Manual_cmdHandlerBase( 
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
      EVENTID_CS_SEQUENCELOADED = 0, /* Sequence file was successfully loaded. */
      EVENTID_CS_SEQUENCECANCELED = 1, /* A command sequence was successfully canceled. */
      EVENTID_CS_FILEREADERROR = 2, /* The Sequence File Loader could not read the sequence file. */
      EVENTID_CS_FILEINVALID = 3, /* The sequence file format was invalid. */
      EVENTID_CS_RECORDINVALID = 4, /* The format of a command record was invalid. */
      EVENTID_CS_FILESIZEERROR = 5, /* The sequence file was too large. */
      EVENTID_CS_FILENOTFOUND = 6, /* The sequence file was not found */
      EVENTID_CS_FILECRCFAILURE = 7, /* The sequence file validation failed */
      EVENTID_CS_COMMANDCOMPLETE = 8, /* The Command Sequencer issued a command and received a success status in return. */
      EVENTID_CS_SEQUENCECOMPLETE = 9, /* A command sequence successfully completed. */
      EVENTID_CS_COMMANDERROR = 10, /* The Command Sequencer issued a command and received an error status in return. */
      EVENTID_CS_INVALIDMODE = 11, /* The Command Sequencer received a command that was invalid for its current mode. */
      EVENTID_CS_RECORDMISMATCH = 12, /* Number of records in header doesn't match number in file */
      EVENTID_CS_TIMEBASEMISMATCH = 13, /* The running time base doesn't match the time base in the sequence files */
      EVENTID_CS_TIMECONTEXTMISMATCH = 14, /* The running time base doesn't match the time base in the sequence files */
      EVENTID_CS_PORTSEQUENCESTARTED = 15, /* A local port request to run a sequence was started */
      EVENTID_CS_UNEXPECTEDCOMPLETION = 16, /* A command status came back when no sequence was running */
      EVENTID_CS_MODESWITCHED = 17, /* Switched step mode */
      EVENTID_CS_NOSEQUENCEACTIVE = 18, /* A sequence related command came with no active sequence */
      EVENTID_CS_SEQUENCEVALID = 19, /* A sequence passed validation */
      EVENTID_CS_SEQUENCETIMEOUT = 20, /* A sequence passed validation */
      EVENTID_CS_CMDSTEPPED = 21, /* A commmand in a sequence was stepped through */
      EVENTID_CS_CMDSTARTED = 22, /* A manual sequence was started */
    };
    
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event enums
    // ----------------------------------------------------------------------

    typedef enum {
      SEQ_READ_HEADER, /*  */
      SEQ_READ_HEADER_SIZE, /*  */
      SEQ_DESER_SIZE, /*  */
      SEQ_DESER_NUM_RECORDS, /*  */
      SEQ_DESER_TIME_BASE, /*  */
      SEQ_DESER_TIME_CONTEXT, /*  */
      SEQ_READ_SEQ_CRC, /*  */
      SEQ_READ_SEQ_DATA, /*  */
      SEQ_READ_SEQ_DATA_SIZE, /*  */
      FileReadStage_MAX
    } FileReadStage;

    typedef enum {
      SEQ_STEP_MODE, /*  */
      SEQ_AUTO_MODE, /*  */
      SeqMode_MAX
    } SeqMode;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event CS_SequenceLoaded
    //!
    /* Sequence file was successfully loaded. */
    void log_ACTIVITY_LO_CS_SequenceLoaded(
        Fw::LogStringArg& fileName /*!< The name of the sequence file*/
    );
    

    //! Log event CS_SequenceCanceled
    //!
    /* A command sequence was successfully canceled. */
    void log_ACTIVITY_HI_CS_SequenceCanceled(
        Fw::LogStringArg& fileName /*!< The name of the sequence file*/
    );
    

    //! Log event CS_FileReadError
    //!
    /* The Sequence File Loader could not read the sequence file. */
    void log_WARNING_HI_CS_FileReadError(
        Fw::LogStringArg& fileName /*!< The name of the sequence file*/
    );
    

    //! Log event CS_FileInvalid
    //!
    /* The sequence file format was invalid. */
    void log_WARNING_HI_CS_FileInvalid(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        FileReadStage stage, /*!< The read stage*/
        I32 error /*!< The error code*/
    );
    

    //! Log event CS_RecordInvalid
    //!
    /* The format of a command record was invalid. */
    void log_WARNING_HI_CS_RecordInvalid(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U32 recordNumber, /*!< The record number*/
        I32 error /*!< The error code*/
    );
    

    //! Log event CS_FileSizeError
    //!
    /* The sequence file was too large. */
    void log_WARNING_HI_CS_FileSizeError(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U32 size /*!< Inavalid size*/
    );
    

    //! Log event CS_FileNotFound
    //!
    /* The sequence file was not found */
    void log_WARNING_HI_CS_FileNotFound(
        Fw::LogStringArg& fileName /*!< The sequence file*/
    );
    

    //! Log event CS_FileCrcFailure
    //!
    /* The sequence file validation failed */
    void log_WARNING_HI_CS_FileCrcFailure(
        Fw::LogStringArg& fileName, /*!< The sequence file*/
        U32 storedCRC, /*!< The CRC stored in the file*/
        U32 computedCRC /*!< The CRC computed over the file*/
    );
    

    //! Log event CS_CommandComplete
    //!
    /* The Command Sequencer issued a command and received a success status in return. */
    void log_ACTIVITY_LO_CS_CommandComplete(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U32 recordNumber, /*!< The record number of the command*/
        U32 opCode /*!< The command opcode*/
    );
    

    //! Log event CS_SequenceComplete
    //!
    /* A command sequence successfully completed. */
    void log_ACTIVITY_HI_CS_SequenceComplete(
        Fw::LogStringArg& fileName /*!< The name of the sequence file*/
    );
    

    //! Log event CS_CommandError
    //!
    /* The Command Sequencer issued a command and received an error status in return. */
    void log_WARNING_HI_CS_CommandError(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U32 recordNumber, /*!< The record number*/
        U32 opCode, /*!< The opcode*/
        U32 errorStatus /*!< The error status*/
    );
    

    //! Log event CS_InvalidMode
    //!
    /* The Command Sequencer received a command that was invalid for its current mode. */
    void log_WARNING_HI_CS_InvalidMode(
        void
    );
    

    //! Log event CS_RecordMismatch
    //!
    /* Number of records in header doesn't match number in file */
    void log_WARNING_HI_CS_RecordMismatch(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U32 header_records, /*!< The number of records in the header*/
        U32 extra_bytes /*!< The number of bytes beyond last record*/
    );
    

    //! Log event CS_TimeBaseMismatch
    //!
    /* The running time base doesn't match the time base in the sequence files */
    void log_WARNING_HI_CS_TimeBaseMismatch(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U16 time_base, /*!< The current time*/
        U16 seq_time_base /*!< The sequence time base*/
    );
    

    //! Log event CS_TimeContextMismatch
    //!
    /* The running time base doesn't match the time base in the sequence files */
    void log_WARNING_HI_CS_TimeContextMismatch(
        Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
        U8 currTimeBase, /*!< The current time base*/
        U8 seqTimeBase /*!< The sequence time base*/
    );
    

    //! Log event CS_PortSequenceStarted
    //!
    /* A local port request to run a sequence was started */
    void log_ACTIVITY_HI_CS_PortSequenceStarted(
        Fw::LogStringArg& filename /*!< The sequence file*/
    );
    

    //! Log event CS_UnexpectedCompletion
    //!
    /* A command status came back when no sequence was running */
    void log_WARNING_HI_CS_UnexpectedCompletion(
        U32 opcode /*!< The reported opcode*/
    );
    

    //! Log event CS_ModeSwitched
    //!
    /* Switched step mode */
    void log_ACTIVITY_HI_CS_ModeSwitched(
        SeqMode mode /*!< The new mode*/
    );
    

    //! Log event CS_NoSequenceActive
    //!
    /* A sequence related command came with no active sequence */
    void log_WARNING_LO_CS_NoSequenceActive(
        void
    );
    

    //! Log event CS_SequenceValid
    //!
    /* A sequence passed validation */
    void log_ACTIVITY_HI_CS_SequenceValid(
        Fw::LogStringArg& filename /*!< The sequence file*/
    );
    

    //! Log event CS_SequenceTimeout
    //!
    /* A sequence passed validation */
    void log_WARNING_HI_CS_SequenceTimeout(
        Fw::LogStringArg& filename, /*!< The sequence file*/
        U32 command /*!< The command that timed out*/
    );
    

    //! Log event CS_CmdStepped
    //!
    /* A commmand in a sequence was stepped through */
    void log_ACTIVITY_HI_CS_CmdStepped(
        Fw::LogStringArg& filename, /*!< The sequence file*/
        U32 command /*!< The command that was stepped*/
    );
    

    //! Log event CS_CmdStarted
    //!
    /* A manual sequence was started */
    void log_ACTIVITY_HI_CS_CmdStarted(
        Fw::LogStringArg& filename /*!< The sequence file*/
    );
    

  PROTECTED:

    // ----------------------------------------------------------------------
    // Channel IDs
    // ----------------------------------------------------------------------

    enum {
      CHANNELID_CS_LOADCOMMANDS = 0, //!< Channel ID for CS_LoadCommands
      CHANNELID_CS_CANCELCOMMANDS = 1, //!< Channel ID for CS_CancelCommands
      CHANNELID_CS_ERRORS = 2, //!< Channel ID for CS_Errors
      CHANNELID_CS_COMMANDSEXECUTED = 3, //!< Channel ID for CS_CommandsExecuted
      CHANNELID_CS_SEQUENCESCOMPLETED = 4, //!< Channel ID for CS_SequencesCompleted
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Telemetry write functions
    // ----------------------------------------------------------------------

    //! Write telemetry channel CS_LoadCommands
    //!
    /* The number of Load commands executed */
    void tlmWrite_CS_LoadCommands(
        U32 arg /*!< The telemetry value*/
    );

    //! Write telemetry channel CS_CancelCommands
    //!
    /* The number of Cancel commands executed */
    void tlmWrite_CS_CancelCommands(
        U32 arg /*!< The telemetry value*/
    );

    //! Write telemetry channel CS_Errors
    //!
    /* The number of errors that have occurred */
    void tlmWrite_CS_Errors(
        U32 arg /*!< The telemetry value*/
    );

    //! Write telemetry channel CS_CommandsExecuted
    //!
    /* The number of commands executed across all sequences. */
    void tlmWrite_CS_CommandsExecuted(
        U32 arg /*!< The telemetry value*/
    );

    //! Write telemetry channel CS_SequencesCompleted
    //!
    /* The number of sequences completed. */
    void tlmWrite_CS_SequencesCompleted(
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

    //! Input port pingIn
    //!
    Svc::InputPingPort m_pingIn_InputPort[NUM_PINGIN_INPUT_PORTS];

    //! Input port cmdResponseIn
    //!
    Fw::InputCmdResponsePort m_cmdResponseIn_InputPort[NUM_CMDRESPONSEIN_INPUT_PORTS];

    //! Input port cmdIn
    //!
    Fw::InputCmdPort m_cmdIn_InputPort[NUM_CMDIN_INPUT_PORTS];

    //! Input port schedIn
    //!
    Svc::InputSchedPort m_schedIn_InputPort[NUM_SCHEDIN_INPUT_PORTS];

    //! Input port seqRunIn
    //!
    Svc::InputCmdSeqInPort m_seqRunIn_InputPort[NUM_SEQRUNIN_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port cmdRegOut
    //!
    Fw::OutputCmdRegPort m_cmdRegOut_OutputPort[NUM_CMDREGOUT_OUTPUT_PORTS];

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Output port LogText
    //!
    Fw::OutputLogTextPort m_LogText_OutputPort[NUM_LOGTEXT_OUTPUT_PORTS];
#endif

    //! Output port tlmOut
    //!
    Fw::OutputTlmPort m_tlmOut_OutputPort[NUM_TLMOUT_OUTPUT_PORTS];

    //! Output port cmdResponseOut
    //!
    Fw::OutputCmdResponsePort m_cmdResponseOut_OutputPort[NUM_CMDRESPONSEOUT_OUTPUT_PORTS];

    //! Output port timeCaller
    //!
    Fw::OutputTimePort m_timeCaller_OutputPort[NUM_TIMECALLER_OUTPUT_PORTS];

    //! Output port comCmdOut
    //!
    Fw::OutputComPort m_comCmdOut_OutputPort[NUM_COMCMDOUT_OUTPUT_PORTS];

    //! Output port pingOut
    //!
    Svc::OutputPingPort m_pingOut_OutputPort[NUM_PINGOUT_OUTPUT_PORTS];

    //! Output port logOut
    //!
    Fw::OutputLogPort m_logOut_OutputPort[NUM_LOGOUT_OUTPUT_PORTS];

    //! Output port seqDone
    //!
    Fw::OutputCmdResponsePort m_seqDone_OutputPort[NUM_SEQDONE_OUTPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port pingIn
    //!
    static void m_p_pingIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Callback for port cmdResponseIn
    //!
    static void m_p_cmdResponseIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CommandResponse response /*!< The command response argument*/
    );

    //! Callback for port cmdIn
    //!
    static void m_p_cmdIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< Command Op Code*/
        U32 cmdSeq, /*!< Command Sequence*/
        Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
    );

    //! Callback for port schedIn
    //!
    static void m_p_schedIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Callback for port seqRunIn
    //!
    static void m_p_seqRunIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::EightyCharString &filename /*!< The sequence file*/
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
    // Counter values for event throttling
    // ----------------------------------------------------------------------

  };  

} // end namespace Svc
#endif
