// ====================================================================== 
// \title  HealthComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for Health component base class
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

#ifndef HEALTH_COMP_HPP_
#define HEALTH_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Svc/Sched/SchedPortAc.hpp>
#include <Svc/Ping/PingPortAc.hpp>
#include <Svc/WatchDog/WatchDogPortAc.hpp>
#include <Fw/Cmd/CmdPortAc.hpp>
#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include <Fw/Cmd/CmdRegPortAc.hpp>
#include <Fw/Tlm/TlmPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif

namespace Svc {

  //! \class HealthComponentBase
  //! \brief Auto-generated base for Health component
  //!
  class HealthComponentBase :
    public Fw::QueuedComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class HealthComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return PingReturn[portNum]
    //!
    Svc::InputPingPort* get_PingReturn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return Run[portNum]
    //!
    Svc::InputSchedPort* get_Run_InputPort(
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

    //! Connect port to PingSend[portNum]
    //!
    void set_PingSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputPingPort *port /*!< The port*/
    );

    //! Connect port to WdogStroke[portNum]
    //!
    void set_WdogStroke_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputWatchDogPort *port /*!< The port*/
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

    //! Connect port to PingSend[portNum]
    //!
    void set_PingSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to WdogStroke[portNum]
    //!
    void set_WdogStroke_OutputPort(
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
    //! Construct a HealthComponentBase object
    //!
    HealthComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a HealthComponentBase object
    //!
    HealthComponentBase(void);
#endif

    //! Initialize a HealthComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a HealthComponentBase object
    //!
    virtual ~HealthComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port PingReturn
    //
    virtual void PingReturn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    ) = 0;

    //! Handler for input port Run
    //
    virtual void Run_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port PingReturn
    //!
    void PingReturn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Handler base-class function for input port Run
    //!
    void Run_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port PingReturn
    //!
    virtual void PingReturn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port PingSend
    //!
    void PingSend_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Invoke output port WdogStroke
    //!
    void WdogStroke_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 code /*!< Watchdog stroke code*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of PingReturn input ports
    //!
    //! \return The number of PingReturn input ports
    //!
    NATIVE_INT_TYPE getNum_PingReturn_InputPorts(void);

    //! Get the number of Run input ports
    //!
    //! \return The number of Run input ports
    //!
    NATIVE_INT_TYPE getNum_Run_InputPorts(void);

    //! Get the number of CmdDisp input ports
    //!
    //! \return The number of CmdDisp input ports
    //!
    NATIVE_INT_TYPE getNum_CmdDisp_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_PINGRETURN_INPUT_PORTS = 25,
       NUM_RUN_INPUT_PORTS = 1,
       NUM_CMDDISP_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of PingSend output ports
    //!
    //! \return The number of PingSend output ports
    //!
    NATIVE_INT_TYPE getNum_PingSend_OutputPorts(void);

    //! Get the number of WdogStroke output ports
    //!
    //! \return The number of WdogStroke output ports
    //!
    NATIVE_INT_TYPE getNum_WdogStroke_OutputPorts(void);

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
       NUM_PINGSEND_OUTPUT_PORTS = 25,
       NUM_WDOGSTROKE_OUTPUT_PORTS = 1,
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

    //! Check whether port PingSend is connected
    //!
    //! \return Whether port PingSend is connected
    //!
    bool isConnected_PingSend_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port WdogStroke is connected
    //!
    //! \return Whether port WdogStroke is connected
    //!
    bool isConnected_WdogStroke_OutputPort(
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
    // Message dispatch
    // ----------------------------------------------------------------------

    //! Dispatch a message on the queue
    //!
    virtual MsgDispatchStatus doDispatch(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command enums
    // ----------------------------------------------------------------------

    typedef enum {
      HLTH_CHK_DISABLED, /*  */
      HLTH_CHK_ENABLED, /*  */
      HealthEnabled_MAX
    } HealthEnabled;

    typedef enum {
      HLTH_PING_DISABLED, /*  */
      HLTH_PING_ENABLED, /*  */
      PingEnabled_MAX
    } PingEnabled;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command opcodes
    // ----------------------------------------------------------------------

    enum {
      OPCODE_HLTH_ENABLE = 0x0, /* A command to enable or disable health checks */
      OPCODE_HLTH_PING_ENABLE = 0x1, /* Ignore a particular ping entry */
      OPCODE_HLTH_CHNG_PING = 0x2, /* Change ping value */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handlers to implement
    // ----------------------------------------------------------------------

    //! Handler for command HLTH_ENABLE
    /* A command to enable or disable health checks */
    virtual void HLTH_ENABLE_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        HealthEnabled enable /*!< whether or not health checks are enabled*/
    ) = 0;

    //! Handler for command HLTH_PING_ENABLE
    /* Ignore a particular ping entry */
    virtual void HLTH_PING_ENABLE_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        const Fw::CmdStringArg& entry, /*!< The entry to enable/disable*/
        PingEnabled enable /*!< whether or not a port is pinged*/
    ) = 0;

    //! Handler for command HLTH_CHNG_PING
    /* Change ping value */
    virtual void HLTH_CHNG_PING_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        const Fw::CmdStringArg& entry, /*!< The entry to modify*/
        U32 warningValue, /*!< Ping warning threshold*/
        U32 fatalValue /*!< Ping fatal threshold*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for async commands.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing the
    // corresponding command. By default they do nothing. You can
    // override them to provide specific pre-command behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for command HLTH_ENABLE
    //!
    virtual void HLTH_ENABLE_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command HLTH_PING_ENABLE
    //!
    virtual void HLTH_PING_ENABLE_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

    //! Pre-message hook for command HLTH_CHNG_PING
    //!
    virtual void HLTH_CHNG_PING_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handler base-class functions.
    // Call these functions directly to bypass the command input port.
    // ----------------------------------------------------------------------

    //! Base-class handler function for command HLTH_ENABLE
    //! 
    void HLTH_ENABLE_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command HLTH_PING_ENABLE
    //! 
    void HLTH_PING_ENABLE_cmdHandlerBase( 
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

    //! Base-class handler function for command HLTH_CHNG_PING
    //! 
    void HLTH_CHNG_PING_cmdHandlerBase( 
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
      EVENTID_HLTH_PING_WARN = 0x0, /* Warn that a ping target is longer than the warning value */
      EVENTID_HLTH_PING_LATE = 0x1, /* Declare FATAL since task is no longer responding */
      EVENTID_HLTH_PING_WRONG_KEY = 0x2, /* Declare FATAL since task is no longer responding */
      EVENTID_HLTH_CHECK_ENABLE = 0x3, /* Report checking turned on or off */
      EVENTID_HLTH_CHECK_PING = 0x4, /* Report a particular entry on or off */
      EVENTID_HLTH_CHECK_LOOKUP_ERROR = 0x5, /* Entry was not found */
      EVENTID_HLTH_PING_UPDATED = 0x6, /* Report changed ping */
      EVENTID_HLTH_PING_INVALID_VALUES = 0x7, /* Report changed ping */
    };
    
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event enums
    // ----------------------------------------------------------------------

    typedef enum {
      HEALTH_CHECK_DISABLED, /*  */
      HEALTH_CHECK_ENABLED, /*  */
      HealthIsEnabled_MAX
    } HealthIsEnabled;

    typedef enum {
      HEALTH_PING_DISABLED, /*  */
      HEALTH_PING_ENABLED, /*  */
      HealthPingIsEnabled_MAX
    } HealthPingIsEnabled;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event HLTH_PING_WARN
    //!
    /* Warn that a ping target is longer than the warning value */
    void log_WARNING_HI_HLTH_PING_WARN(
        Fw::LogStringArg& entry /*!< The entry passing the warning level*/
    );
    

    //! Log event HLTH_PING_LATE
    //!
    /* Declare FATAL since task is no longer responding */
    void log_FATAL_HLTH_PING_LATE(
        Fw::LogStringArg& entry /*!< The entry passing the warning level*/
    );
    

    //! Log event HLTH_PING_WRONG_KEY
    //!
    /* Declare FATAL since task is no longer responding */
    void log_FATAL_HLTH_PING_WRONG_KEY(
        Fw::LogStringArg& entry, /*!< The entry passing the warning level*/
        U32 badKey /*!< The incorrect key value*/
    );
    

    //! Log event HLTH_CHECK_ENABLE
    //!
    /* Report checking turned on or off */
    void log_ACTIVITY_HI_HLTH_CHECK_ENABLE(
        HealthIsEnabled enabled /*!< If health checking is enabled*/
    );
    

    //! Log event HLTH_CHECK_PING
    //!
    /* Report a particular entry on or off */
    void log_ACTIVITY_HI_HLTH_CHECK_PING(
        HealthPingIsEnabled enabled, /*!< If health pinging is enabled for a particular entry*/
        Fw::LogStringArg& entry /*!< The entry passing the warning level*/
    );
    

    //! Log event HLTH_CHECK_LOOKUP_ERROR
    //!
    /* Entry was not found */
    void log_WARNING_LO_HLTH_CHECK_LOOKUP_ERROR(
        Fw::LogStringArg& entry /*!< The entry passing the warning level*/
    );
    

    //! Log event HLTH_PING_UPDATED
    //!
    /* Report changed ping */
    void log_ACTIVITY_HI_HLTH_PING_UPDATED(
        Fw::LogStringArg& entry, /*!< The entry changed*/
        U32 warn, /*!< The new warning value*/
        U32 fatal /*!< The new FATAL value*/
    );
    

    //! Log event HLTH_PING_INVALID_VALUES
    //!
    /* Report changed ping */
    void log_WARNING_HI_HLTH_PING_INVALID_VALUES(
        Fw::LogStringArg& entry, /*!< The entry changed*/
        U32 warn, /*!< The new warning value*/
        U32 fatal /*!< The new FATAL value*/
    );
    

  PROTECTED:

    // ----------------------------------------------------------------------
    // Channel IDs
    // ----------------------------------------------------------------------

    enum {
      CHANNELID_PINGLATEWARNINGS = 0x0, //!< Channel ID for PingLateWarnings
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Telemetry write functions
    // ----------------------------------------------------------------------

    //! Write telemetry channel PingLateWarnings
    //!
    /* Number of overrun warnings */
    void tlmWrite_PingLateWarnings(
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

    //! Input port PingReturn
    //!
    Svc::InputPingPort m_PingReturn_InputPort[NUM_PINGRETURN_INPUT_PORTS];

    //! Input port Run
    //!
    Svc::InputSchedPort m_Run_InputPort[NUM_RUN_INPUT_PORTS];

    //! Input port CmdDisp
    //!
    Fw::InputCmdPort m_CmdDisp_InputPort[NUM_CMDDISP_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port PingSend
    //!
    Svc::OutputPingPort m_PingSend_OutputPort[NUM_PINGSEND_OUTPUT_PORTS];

    //! Output port WdogStroke
    //!
    Svc::OutputWatchDogPort m_WdogStroke_OutputPort[NUM_WDOGSTROKE_OUTPUT_PORTS];

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

    //! Callback for port PingReturn
    //!
    static void m_p_PingReturn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

    //! Callback for port Run
    //!
    static void m_p_Run_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
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
    // Counter values for event throttling
    // ----------------------------------------------------------------------

  };  

} // end namespace Svc
#endif
