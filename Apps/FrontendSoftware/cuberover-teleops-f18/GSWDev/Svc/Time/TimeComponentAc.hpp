// ====================================================================== 
// \title  TimeComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for Time component base class
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

#ifndef TIME_COMP_HPP_
#define TIME_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Time/TimePortAc.hpp>

namespace Svc {

  //! \class TimeComponentBase
  //! \brief Auto-generated base for Time component
  //!
  class TimeComponentBase :
    public Fw::PassiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class TimeComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return timeGetPort[portNum]
    //!
    Fw::InputTimePort* get_timeGetPort_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a TimeComponentBase object
    //!
    TimeComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a TimeComponentBase object
    //!
    TimeComponentBase(void);
#endif

    //! Initialize a TimeComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a TimeComponentBase object
    //!
    virtual ~TimeComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port timeGetPort
    //
    virtual void timeGetPort_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Time &time /*!< The U32 cmd argument*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port timeGetPort
    //!
    void timeGetPort_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Time &time /*!< The U32 cmd argument*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of timeGetPort input ports
    //!
    //! \return The number of timeGetPort input ports
    //!
    NATIVE_INT_TYPE getNum_timeGetPort_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_TIMEGETPORT_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------


  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed input ports
    // ----------------------------------------------------------------------

    //! Input port timeGetPort
    //!
    Fw::InputTimePort m_timeGetPort_InputPort[NUM_TIMEGETPORT_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port timeGetPort
    //!
    static void m_p_timeGetPort_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Time &time /*!< The U32 cmd argument*/
    );



  };  

} // end namespace Svc
#endif
