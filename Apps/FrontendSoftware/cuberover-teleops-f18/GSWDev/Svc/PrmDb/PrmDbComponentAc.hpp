// ====================================================================== 
// \title  PrmDbComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for PrmDb component base class
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

#ifndef PRMDB_COMP_HPP_
#define PRMDB_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Os/Mutex.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Prm/PrmBuffer.hpp>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Prm/PrmGetPortAc.hpp>
#include <Fw/Prm/PrmSetPortAc.hpp>
#include <Svc/Ping/PingPortAc.hpp>
#include <Fw/Cmd/CmdPortAc.hpp>
#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include <Fw/Cmd/CmdRegPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif

namespace Svc {

  //! \class PrmDbComponentBase
  //! \brief Auto-generated base for PrmDb component
  //!
  class PrmDbComponentBase :
    public Fw::ActiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class PrmDbComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return getPrm[portNum]
    //!
    Fw::InputPrmGetPort* get_getPrm_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return setPrm[portNum]
    //!
    Fw::InputPrmSetPort* get_setPrm_InputPort(
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
    //! Construct a PrmDbComponentBase object
    //!
    PrmDbComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a PrmDbComponentBase object
    //!
    PrmDbComponentBase(void);
#endif

    //! Initialize a PrmDbComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a PrmDbComponentBase object
    //!
    virtual ~PrmDbComponentBase(void);

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

    //! Handler for input port getPrm
    //
    virtual Fw::ParamValid getPrm_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
    ) = 0;

    //! Handler for input port setPrm
    //
    virtual void setPrm_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
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

    //! Handler base-class function for input port getPrm
    //!
    Fw::ParamValid getPrm_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
    );

    //! Handler base-class function for input port setPrm
    //!
    void setPrm_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
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

    //! Pre-message hook for async input port setPrm
    //!
    virtual void setPrm_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
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

    //! Get the number of getPrm input ports
    //!
    //! \return The number of getPrm input ports
    //!
    NATIVE_INT_TYPE getNum_getPrm_InputPorts(void);

    //! Get the number of setPrm input ports
    //!
    //! \return The number of setPrm input ports
    //!
    NATIVE_INT_TYPE getNum_setPrm_InputPorts(void);

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
       NUM_GETPRM_INPUT_PORTS = 1,
       NUM_SETPRM_INPUT_PORTS = 1,
       NUM_PINGIN_INPUT_PORTS = 1,
       NUM_CMDDISP_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

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
       NUM_PINGOUT_OUTPUT_PORTS = 1,
       NUM_CMDSTATUS_OUTPUT_PORTS = 1,
       NUM_CMDREG_OUTPUT_PORTS = 1,
       NUM_TIME_OUTPUT_PORTS = 1,
       NUM_LOG_OUTPUT_PORTS = 1,
       NUM_LOGTEXT_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

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
      OPCODE_PRM_SAVE_FILE = 0, /* Command to save parameter image to file. Uses file name passed to constructor */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handlers to implement
    // ----------------------------------------------------------------------

    //! Handler for command PRM_SAVE_FILE
    /* Command to save parameter image to file. Uses file name passed to constructor */
    virtual void PRM_SAVE_FILE_cmdHandler(
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

    //! Pre-message hook for command PRM_SAVE_FILE
    //!
    virtual void PRM_SAVE_FILE_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handler base-class functions.
    // Call these functions directly to bypass the command input port.
    // ----------------------------------------------------------------------

    //! Base-class handler function for command PRM_SAVE_FILE
    //! 
    void PRM_SAVE_FILE_cmdHandlerBase( 
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
      EVENTID_PRMIDNOTFOUND = 0, /* Parameter ID not found in database. */
      EVENTID_PRMIDUPDATED = 1, /* Parameter ID updated in database */
      EVENTID_PRMDBFULL = 2, /* Parameter database is full */
      EVENTID_PRMIDADDED = 3, /* Parameter ID added to database */
      EVENTID_PRMFILEWRITEERROR = 4, /* Failed to write parameter file */
      EVENTID_PRMFILESAVECOMPLETE = 5, /* Save of parameter file completed */
      EVENTID_PRMFILEREADERROR = 6, /* Failed to read parameter file */
      EVENTID_PRMFILELOADCOMPLETE = 7, /* Load of parameter file completed */
    };
    
    // ----------------------------------------------------------------------
    // Event Throttle values - sets initial value of countdown variable
    // ----------------------------------------------------------------------

    enum {
      EVENTID_PRMIDNOTFOUND_THROTTLE = 5, /*!< Throttle reset count for PrmIdNotFound*/
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event enums
    // ----------------------------------------------------------------------

    typedef enum {
      PRM_WRITE_OPEN, /*  */
      PRM_WRITE_DELIMETER, /*  */
      PRM_WRITE_DELIMETER_SIZE, /*  */
      PRM_WRITE_RECORD_SIZE, /*  */
      PRM_WRITE_RECORD_SIZE_SIZE, /*  */
      PRM_WRITE_PARAMETER_ID, /*  */
      PRM_WRITE_PARAMETER_ID_SIZE, /*  */
      PRM_WRITE_PARAMETER_VALUE, /*  */
      PRM_WRITE_PARAMETER_VALUE_SIZE, /*  */
      PrmWriteError_MAX
    } PrmWriteError;

    typedef enum {
      PRM_READ_OPEN, /*  */
      PRM_READ_DELIMETER, /*  */
      PRM_READ_DELIMETER_SIZE, /*  */
      PRM_READ_DELIMETER_VALUE, /*  */
      PRM_READ_RECORD_SIZE, /*  */
      PRM_READ_RECORD_SIZE_SIZE, /*  */
      PRM_READ_RECORD_SIZE_VALUE, /*  */
      PRM_READ_PARAMETER_ID, /*  */
      PRM_READ_PARAMETER_ID_SIZE, /*  */
      PRM_READ_PARAMETER_VALUE, /*  */
      PRM_READ_PARAMETER_VALUE_SIZE, /*  */
      PrmReadError_MAX
    } PrmReadError;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event PrmIdNotFound
    //!
    /* Parameter ID not found in database. */
    void log_WARNING_LO_PrmIdNotFound(
        U32 Id /*!< The parameter ID*/
    );
    
    // reset throttle value for PrmIdNotFound
    void log_WARNING_LO_PrmIdNotFound_ThrottleClear(void);

    //! Log event PrmIdUpdated
    //!
    /* Parameter ID updated in database */
    void log_ACTIVITY_HI_PrmIdUpdated(
        U32 Id /*!< The parameter ID*/
    );
    

    //! Log event PrmDbFull
    //!
    /* Parameter database is full */
    void log_FATAL_PrmDbFull(
        U32 Id /*!< The parameter ID*/
    );
    

    //! Log event PrmIdAdded
    //!
    /* Parameter ID added to database */
    void log_ACTIVITY_HI_PrmIdAdded(
        U32 Id /*!< The parameter ID*/
    );
    

    //! Log event PrmFileWriteError
    //!
    /* Failed to write parameter file */
    void log_WARNING_HI_PrmFileWriteError(
        PrmWriteError stage, /*!< The write stage*/
        I32 record, /*!< The record that had the failure*/
        I32 error /*!< The error code*/
    );
    

    //! Log event PrmFileSaveComplete
    //!
    /* Save of parameter file completed */
    void log_ACTIVITY_HI_PrmFileSaveComplete(
        U32 records /*!< The number of records saved*/
    );
    

    //! Log event PrmFileReadError
    //!
    /* Failed to read parameter file */
    void log_WARNING_HI_PrmFileReadError(
        PrmReadError stage, /*!< The write stage*/
        I32 record, /*!< The record that had the failure*/
        I32 error /*!< The error code*/
    );
    

    //! Log event PrmFileLoadComplete
    //!
    /* Load of parameter file completed */
    void log_ACTIVITY_HI_PrmFileLoadComplete(
        U32 records /*!< The number of records loaded*/
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

    //! Input port getPrm
    //!
    Fw::InputPrmGetPort m_getPrm_InputPort[NUM_GETPRM_INPUT_PORTS];

    //! Input port setPrm
    //!
    Fw::InputPrmSetPort m_setPrm_InputPort[NUM_SETPRM_INPUT_PORTS];

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

    //! Output port pingOut
    //!
    Svc::OutputPingPort m_pingOut_OutputPort[NUM_PINGOUT_OUTPUT_PORTS];

    //! Output port CmdStatus
    //!
    Fw::OutputCmdResponsePort m_CmdStatus_OutputPort[NUM_CMDSTATUS_OUTPUT_PORTS];

    //! Output port CmdReg
    //!
    Fw::OutputCmdRegPort m_CmdReg_OutputPort[NUM_CMDREG_OUTPUT_PORTS];

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

    //! Callback for port getPrm
    //!
    static Fw::ParamValid m_p_getPrm_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
    );

    //! Callback for port setPrm
    //!
    static void m_p_setPrm_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwPrmIdType id, /*!< Parameter ID*/
        Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
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
    // Counter values for event throttling
    // ----------------------------------------------------------------------
    NATIVE_UINT_TYPE m_PrmIdNotFoundThrottle; //!< throttle for PrmIdNotFound

  };  

} // end namespace Svc
#endif
