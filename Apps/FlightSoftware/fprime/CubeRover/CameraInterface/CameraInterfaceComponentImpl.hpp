// ======================================================================
// \title  CameraInterfaceComponentImpl.hpp
// \author cedric
// \brief  hpp file for CameraInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CameraInterface_HPP
#define CameraInterface_HPP

#include "CubeRover/CameraInterface/CameraInterfaceComponentAc.hpp"

namespace CubeRover {

  namespace S25fl064l{
    typedef enum FlashSpiRegister{

    };
  }

  typedef uint32_t  CameraError;

  class CameraInterfaceComponentImpl :
    public CameraInterfaceComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object CameraInterface
      //!
      CameraInterfaceComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object CameraInterface
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object CameraInterface
      //!
      ~CameraInterfaceComponentImpl(void);

    CameraError flashReadData(const S25fl064l::FlashSpiRegister regStartAddr, 
                              uint16_t *rxData,
                              const uint8_t length);

    CameraError flashWriteData(const S25fl064l::FlashSpiRegister regStartAddr,
                               uint16_t *txData, 
                               const uint8_t length);
    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for CAM_CaptureImage command handler
      //! Capture image from camera
      void CAM_CaptureImage_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CAM_FetchImage command handler
      //! Fetch an image from the camera
      void CAM_FetchImage_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    private:
      m_flashDataConfig;

    };

} // end namespace CubeRover

#endif
