// ======================================================================
// \title  Wf121DriverComponentImpl.hpp
// \author cedric
// \brief  hpp file for Wf121Driver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Wf121Driver_HPP
#define Wf121Driver_HPP

#include "CubeRover/Wf121/Wf121ComponentAc.hpp"

namespace CubeRover {

  class Wf121DriverComponentImpl :
    public Wf121DriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Wf121Driver
      //!
      Wf121DriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Wf121Driver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Wf121Driver
      //!
      ~Wf121DriverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Wf121ReadWrite
      //!
      void Wf121ReadWrite_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &writeBuffer, 
          Fw::Buffer &readBuffer 
      );


    };

} // end namespace CubeRover

#endif
