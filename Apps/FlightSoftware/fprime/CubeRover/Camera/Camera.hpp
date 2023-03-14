// ======================================================================
// \title  CameraComponentImpl.hpp
// \author Raewyn
// \brief  hpp file for Camera component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Camera_HPP
#define Camera_HPP

#include "CubeRover/Camera/CameraComponentAc.hpp"
#include "CubeRover/NetworkManager/NetworkManager.hpp" // ! TODO: FIXME Hacky patch connection to steal NM data
#include <CubeRover/Camera/CameraTask.hpp>

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"
#include "os_task.h"
#include <Os/Mutex.hpp>

namespace CubeRover {

    // These three parameters control the setup of the task that handles sending UDP data to the WF121 Radio:
    static const NATIVE_INT_TYPE CAMERA_TASK_PRIORITY = 9; // Less than all comm layers
    // NOTE: Stack size is in words. Make sure there's enough room for the overhead (min task size) plus some overhead. Use `uxTaskGetStackHighWaterMark(NULL)` to tune.
    static const NATIVE_INT_TYPE CAMERA_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE + 256; // **Shouldn't** be handling anything big dynamicly, so can be kind of shallow
    static const NATIVE_INT_TYPE CAMERA_TASK_CPU_AFFINITY = -1;

  class CameraComponentImpl :
    public CameraComponentBase, public Camera::CameraDownlinkManager
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Camera
      //!
      CameraComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Camera
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Camera
      //!
      ~CameraComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for takeImage
      //!
      void takePicture_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U8 CameraNum, /*!< Camera Number*/
          U16 CallbackId /*!< Identifier downlinked with the image allowing us to map the command which triggered image capture to this image*/
      );

    PRIVATE:
        //! Handler implementation for schedIn
        //!
        void schedIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            NATIVE_UINT_TYPE context       /*!< The call order*/
        );

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Get_Status command handler
      //! Get camera status
      void Get_Status_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for Take_Image command handler
      //! Take an Image
      void Take_Image_cmdHandler(
              const FwOpcodeType opCode, /*!< The opcode*/
              const U32 cmdSeq, /*!< The command sequence number*/
              U8 camera_num, /*!<
                              0: Camera 0     1: Camera 1, etc.
                          */
              U16 callback_id, /*!<
                              Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                          */
              U16 skipXPairs,
              U16 skipYPairs,
              U16 startXPairs,
              U16 startYPairs,
              U16 endXPairs,
              U16 endYPairs
          );

      //! Implementation for Error command handler
      //! Get camera status
      void Error_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 action /*!< 
                        0x00: Clear Error     0xff: Read Error  !! Not sure how this is being used
                    */
      );

      //! Implementation for Configure_Camera0 command handler
      //! Configure camera 0
      void Configure_Camera0_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U64 config /*!< 
                        Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                    */
      );

      //! Implementation for Camera0_Crop command handler
      //! Defines the crop for camera 0
      void Camera0_Crop_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U64 config /*!< 
                        Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                    */
      );

      //! Implementation for Configure_Camera1 command handler
      //! Configure camera 1
      void Configure_Camera1_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U64 config /*!< 
                        Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                    */
      );

      //! Implementation for Camera1_Crop command handler
      //! Defines the crop for camera 1
      void Camera1_Crop_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U64 config /*!< 
                        Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                    */
      );

      //! Implementation for Read_Image command handler
      //! Downlink image generated by a sequence number
      void Read_Image_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 callbackID /*!< 
                        DRedownlink an image generated from the specified callbac
                    */
      );

      //! Implementation for Erase_Image command handler
      //! TBD
      void Erase_Image_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for Soft_Camera_Reset command handler
      //! Does a soft reset of the camera (and something with the FPGA)
      void Soft_Camera_Reset_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 cameraNum /*!< 
                        Camera Number
                    */
      );

      //! Implementation for Image_Dump command handler
      //! TBD
      void Image_Dump_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );
      
      
      // ----------------------------------------------------------------------
      // User Methods
      // ----------------------------------------------------------------------

      void triggerImageCapture(uint8_t camera, uint16_t callbackId);
      void downlinkImage(uint8_t *image, int size, uint16_t callbackId, uint32_t captureTime, uint16_t downlinkLineNumber, uint16_t totalDownlinkLineCount);

      // FreeRTOS Task responsible for handling camera I/O:
      Camera::CameraTask m_cameraTask;

      U32 m_numComponentImgsReq;
      U32 m_numGroundImgsReq;
      U32 m_imagesSent;
      U32 m_bytesSent;

    };

} // end namespace CubeRover

#endif
