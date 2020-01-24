// ====================================================================== 
// \title  BuffGndSockIfComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for BuffGndSockIf component base class
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

#ifndef BUFFGNDSOCKIF_COMP_HPP_
#define BUFFGNDSOCKIF_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Buffer/BufferSerializableAc.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Log/LogPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Buffer/BufferSendPortAc.hpp>
#include <Fw/Buffer/BufferGetPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif

namespace Svc {

  //! \class BuffGndSockIfComponentBase
  //! \brief Auto-generated base for BuffGndSockIf component
  //!
  class BuffGndSockIfComponentBase :
    public Fw::PassiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class BuffGndSockIfComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return downlink[portNum]
    //!
    Fw::InputBufferSendPort* get_downlink_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to Log[portNum]
    //!
    void set_Log_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogPort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTimePort *port /*!< The port*/
    );

    //! Connect port to bufferReturn[portNum]
    //!
    void set_bufferReturn_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferSendPort *port /*!< The port*/
    );

    //! Connect port to bufferGet[portNum]
    //!
    void set_bufferGet_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferGetPort *port /*!< The port*/
    );

    //! Connect port to uplink[portNum]
    //!
    void set_uplink_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferSendPort *port /*!< The port*/
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

    //! Connect port to Log[portNum]
    //!
    void set_Log_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to bufferReturn[portNum]
    //!
    void set_bufferReturn_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to bufferGet[portNum]
    //!
    void set_bufferGet_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to uplink[portNum]
    //!
    void set_uplink_OutputPort(
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

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a BuffGndSockIfComponentBase object
    //!
    BuffGndSockIfComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a BuffGndSockIfComponentBase object
    //!
    BuffGndSockIfComponentBase(void);
#endif

    //! Initialize a BuffGndSockIfComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a BuffGndSockIfComponentBase object
    //!
    virtual ~BuffGndSockIfComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port downlink
    //
    virtual void downlink_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port downlink
    //!
    void downlink_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port bufferReturn
    //!
    void bufferReturn_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

    //! Invoke output port bufferGet
    //!
    Fw::Buffer bufferGet_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 size 
    );

    //! Invoke output port uplink
    //!
    void uplink_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of downlink input ports
    //!
    //! \return The number of downlink input ports
    //!
    NATIVE_INT_TYPE getNum_downlink_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_DOWNLINK_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of Log output ports
    //!
    //! \return The number of Log output ports
    //!
    NATIVE_INT_TYPE getNum_Log_OutputPorts(void);

    //! Get the number of Time output ports
    //!
    //! \return The number of Time output ports
    //!
    NATIVE_INT_TYPE getNum_Time_OutputPorts(void);

    //! Get the number of bufferReturn output ports
    //!
    //! \return The number of bufferReturn output ports
    //!
    NATIVE_INT_TYPE getNum_bufferReturn_OutputPorts(void);

    //! Get the number of bufferGet output ports
    //!
    //! \return The number of bufferGet output ports
    //!
    NATIVE_INT_TYPE getNum_bufferGet_OutputPorts(void);

    //! Get the number of uplink output ports
    //!
    //! \return The number of uplink output ports
    //!
    NATIVE_INT_TYPE getNum_uplink_OutputPorts(void);

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Get the number of LogText output ports
    //!
    //! \return The number of LogText output ports
    //!
    NATIVE_INT_TYPE getNum_LogText_OutputPorts(void);
#endif

  
    enum {
       NUM_LOG_OUTPUT_PORTS = 1,
       NUM_TIME_OUTPUT_PORTS = 1,
       NUM_BUFFERRETURN_OUTPUT_PORTS = 1,
       NUM_BUFFERGET_OUTPUT_PORTS = 1,
       NUM_UPLINK_OUTPUT_PORTS = 1,
       NUM_LOGTEXT_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port Log is connected
    //!
    //! \return Whether port Log is connected
    //!
    bool isConnected_Log_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port Time is connected
    //!
    //! \return Whether port Time is connected
    //!
    bool isConnected_Time_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port bufferReturn is connected
    //!
    //! \return Whether port bufferReturn is connected
    //!
    bool isConnected_bufferReturn_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port bufferGet is connected
    //!
    //! \return Whether port bufferGet is connected
    //!
    bool isConnected_bufferGet_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port uplink is connected
    //!
    //! \return Whether port uplink is connected
    //!
    bool isConnected_uplink_OutputPort(
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
    // Event IDs
    // ----------------------------------------------------------------------

    enum {
      EVENTID_BUFFGNDSOCKIF_NOCONNECTIONTOSERVER = 0, /* The thread will try to re-connect until a connection is made */
      EVENTID_BUFFGNDSOCKIF_CONNECTEDTOSERVER = 1, /* The socket thread will continuously read until disconnected or terminated */
      EVENTID_BUFFGNDSOCKIF_LOSTCONNECTIONTOSERVER = 2, /* The socket thread will continuously try to reconnect to socket server if connection dropped by server */
    };
    
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event BuffGndSockIf_NoConnectionToServer
    //!
    /* The thread will try to re-connect until a connection is made */
    void log_WARNING_LO_BuffGndSockIf_NoConnectionToServer(
        U32 PortNumber /*!< The port number running the socket server*/
    );
    

    //! Log event BuffGndSockIf_ConnectedToServer
    //!
    /* The socket thread will continuously read until disconnected or terminated */
    void log_ACTIVITY_HI_BuffGndSockIf_ConnectedToServer(
        U32 PortNumber /*!< The port number running the socket server*/
    );
    

    //! Log event BuffGndSockIf_LostConnectionToServer
    //!
    /* The socket thread will continuously try to reconnect to socket server if connection dropped by server */
    void log_WARNING_LO_BuffGndSockIf_LostConnectionToServer(
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

    //! Input port downlink
    //!
    Fw::InputBufferSendPort m_downlink_InputPort[NUM_DOWNLINK_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port Log
    //!
    Fw::OutputLogPort m_Log_OutputPort[NUM_LOG_OUTPUT_PORTS];

    //! Output port Time
    //!
    Fw::OutputTimePort m_Time_OutputPort[NUM_TIME_OUTPUT_PORTS];

    //! Output port bufferReturn
    //!
    Fw::OutputBufferSendPort m_bufferReturn_OutputPort[NUM_BUFFERRETURN_OUTPUT_PORTS];

    //! Output port bufferGet
    //!
    Fw::OutputBufferGetPort m_bufferGet_OutputPort[NUM_BUFFERGET_OUTPUT_PORTS];

    //! Output port uplink
    //!
    Fw::OutputBufferSendPort m_uplink_OutputPort[NUM_UPLINK_OUTPUT_PORTS];

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Output port LogText
    //!
    Fw::OutputLogTextPort m_LogText_OutputPort[NUM_LOGTEXT_OUTPUT_PORTS];
#endif

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port downlink
    //!
    static void m_p_downlink_in(
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
