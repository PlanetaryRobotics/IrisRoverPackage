// ====================================================================== 
// \title  GndIfComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for GndIf component base class
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

#ifndef GNDIF_COMP_HPP_
#define GNDIF_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Buffer/BufferSerializableAc.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Buffer/BufferSendPortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif
#include <Fw/Com/ComPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Buffer/BufferGetPortAc.hpp>

namespace Svc {

  //! \class GndIfComponentBase
  //! \brief Auto-generated base for GndIf component
  //!
  class GndIfComponentBase :
    public Fw::PassiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class GndIfComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return downlinkPort[portNum]
    //!
    Fw::InputComPort* get_downlinkPort_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return fileDownlinkBufferSendIn[portNum]
    //!
    Fw::InputBufferSendPort* get_fileDownlinkBufferSendIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to fileUplinkBufferSendOut[portNum]
    //!
    void set_fileUplinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferSendPort *port /*!< The port*/
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

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTimePort *port /*!< The port*/
    );

    //! Connect port to uplinkPort[portNum]
    //!
    void set_uplinkPort_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputComPort *port /*!< The port*/
    );

    //! Connect port to fileDownlinkBufferSendOut[portNum]
    //!
    void set_fileDownlinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferSendPort *port /*!< The port*/
    );

    //! Connect port to fileUplinkBufferGet[portNum]
    //!
    void set_fileUplinkBufferGet_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferGetPort *port /*!< The port*/
    );

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to fileUplinkBufferSendOut[portNum]
    //!
    void set_fileUplinkBufferSendOut_OutputPort(
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

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to uplinkPort[portNum]
    //!
    void set_uplinkPort_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to fileDownlinkBufferSendOut[portNum]
    //!
    void set_fileDownlinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to fileUplinkBufferGet[portNum]
    //!
    void set_fileUplinkBufferGet_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#endif

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a GndIfComponentBase object
    //!
    GndIfComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a GndIfComponentBase object
    //!
    GndIfComponentBase(void);
#endif

    //! Initialize a GndIfComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a GndIfComponentBase object
    //!
    virtual ~GndIfComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port downlinkPort
    //
    virtual void downlinkPort_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    ) = 0;

    //! Handler for input port fileDownlinkBufferSendIn
    //
    virtual void fileDownlinkBufferSendIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port downlinkPort
    //!
    void downlinkPort_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Handler base-class function for input port fileDownlinkBufferSendIn
    //!
    void fileDownlinkBufferSendIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port fileUplinkBufferSendOut
    //!
    void fileUplinkBufferSendOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

    //! Invoke output port uplinkPort
    //!
    void uplinkPort_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Invoke output port fileDownlinkBufferSendOut
    //!
    void fileDownlinkBufferSendOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

    //! Invoke output port fileUplinkBufferGet
    //!
    Fw::Buffer fileUplinkBufferGet_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 size 
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of downlinkPort input ports
    //!
    //! \return The number of downlinkPort input ports
    //!
    NATIVE_INT_TYPE getNum_downlinkPort_InputPorts(void);

    //! Get the number of fileDownlinkBufferSendIn input ports
    //!
    //! \return The number of fileDownlinkBufferSendIn input ports
    //!
    NATIVE_INT_TYPE getNum_fileDownlinkBufferSendIn_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_DOWNLINKPORT_INPUT_PORTS = 1,
       NUM_FILEDOWNLINKBUFFERSENDIN_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of fileUplinkBufferSendOut output ports
    //!
    //! \return The number of fileUplinkBufferSendOut output ports
    //!
    NATIVE_INT_TYPE getNum_fileUplinkBufferSendOut_OutputPorts(void);

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

    //! Get the number of Time output ports
    //!
    //! \return The number of Time output ports
    //!
    NATIVE_INT_TYPE getNum_Time_OutputPorts(void);

    //! Get the number of uplinkPort output ports
    //!
    //! \return The number of uplinkPort output ports
    //!
    NATIVE_INT_TYPE getNum_uplinkPort_OutputPorts(void);

    //! Get the number of fileDownlinkBufferSendOut output ports
    //!
    //! \return The number of fileDownlinkBufferSendOut output ports
    //!
    NATIVE_INT_TYPE getNum_fileDownlinkBufferSendOut_OutputPorts(void);

    //! Get the number of fileUplinkBufferGet output ports
    //!
    //! \return The number of fileUplinkBufferGet output ports
    //!
    NATIVE_INT_TYPE getNum_fileUplinkBufferGet_OutputPorts(void);

  
    enum {
       NUM_FILEUPLINKBUFFERSENDOUT_OUTPUT_PORTS = 1,
       NUM_LOG_OUTPUT_PORTS = 1,
       NUM_LOGTEXT_OUTPUT_PORTS = 1,
       NUM_TIME_OUTPUT_PORTS = 1,
       NUM_UPLINKPORT_OUTPUT_PORTS = 1,
       NUM_FILEDOWNLINKBUFFERSENDOUT_OUTPUT_PORTS = 1,
       NUM_FILEUPLINKBUFFERGET_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port fileUplinkBufferSendOut is connected
    //!
    //! \return Whether port fileUplinkBufferSendOut is connected
    //!
    bool isConnected_fileUplinkBufferSendOut_OutputPort(
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

    //! Check whether port Time is connected
    //!
    //! \return Whether port Time is connected
    //!
    bool isConnected_Time_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port uplinkPort is connected
    //!
    //! \return Whether port uplinkPort is connected
    //!
    bool isConnected_uplinkPort_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port fileDownlinkBufferSendOut is connected
    //!
    //! \return Whether port fileDownlinkBufferSendOut is connected
    //!
    bool isConnected_fileDownlinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port fileUplinkBufferGet is connected
    //!
    //! \return Whether port fileUplinkBufferGet is connected
    //!
    bool isConnected_fileUplinkBufferGet_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event IDs
    // ----------------------------------------------------------------------

    enum {
      EVENTID_NOCONNECTIONTOSERVER = 0, /* The thread will try to re-connect until a connection is made */
      EVENTID_CONNECTEDTOSERVER = 1, /* The socket thread will continuously read until disconnected or terminated */
      EVENTID_LOSTCONNECTIONTOSERVER = 2, /* The socket thread will continuously try to reconnect to socket server if connection dropped by server */
    };
    
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event NoConnectionToServer
    //!
    /* The thread will try to re-connect until a connection is made */
    void log_WARNING_LO_NoConnectionToServer(
        U32 PortNumber /*!< The port number running the socket server*/
    );
    

    //! Log event ConnectedToServer
    //!
    /* The socket thread will continuously read until disconnected or terminated */
    void log_ACTIVITY_HI_ConnectedToServer(
        U32 PortNumber /*!< The port number running the socket server*/
    );
    

    //! Log event LostConnectionToServer
    //!
    /* The socket thread will continuously try to reconnect to socket server if connection dropped by server */
    void log_WARNING_LO_LostConnectionToServer(
        U32 PortNumber /*!< The port number running the socket server*/
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

    //! Input port downlinkPort
    //!
    Fw::InputComPort m_downlinkPort_InputPort[NUM_DOWNLINKPORT_INPUT_PORTS];

    //! Input port fileDownlinkBufferSendIn
    //!
    Fw::InputBufferSendPort m_fileDownlinkBufferSendIn_InputPort[NUM_FILEDOWNLINKBUFFERSENDIN_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port fileUplinkBufferSendOut
    //!
    Fw::OutputBufferSendPort m_fileUplinkBufferSendOut_OutputPort[NUM_FILEUPLINKBUFFERSENDOUT_OUTPUT_PORTS];

    //! Output port Log
    //!
    Fw::OutputLogPort m_Log_OutputPort[NUM_LOG_OUTPUT_PORTS];

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Output port LogText
    //!
    Fw::OutputLogTextPort m_LogText_OutputPort[NUM_LOGTEXT_OUTPUT_PORTS];
#endif

    //! Output port Time
    //!
    Fw::OutputTimePort m_Time_OutputPort[NUM_TIME_OUTPUT_PORTS];

    //! Output port uplinkPort
    //!
    Fw::OutputComPort m_uplinkPort_OutputPort[NUM_UPLINKPORT_OUTPUT_PORTS];

    //! Output port fileDownlinkBufferSendOut
    //!
    Fw::OutputBufferSendPort m_fileDownlinkBufferSendOut_OutputPort[NUM_FILEDOWNLINKBUFFERSENDOUT_OUTPUT_PORTS];

    //! Output port fileUplinkBufferGet
    //!
    Fw::OutputBufferGetPort m_fileUplinkBufferGet_OutputPort[NUM_FILEUPLINKBUFFERGET_OUTPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port downlinkPort
    //!
    static void m_p_downlinkPort_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Callback for port fileDownlinkBufferSendIn
    //!
    static void m_p_fileDownlinkBufferSendIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );


  PRIVATE:
    // ----------------------------------------------------------------------
    // Counter values for event throttling
    // ----------------------------------------------------------------------

  };  

} // end namespace Svc
#endif
