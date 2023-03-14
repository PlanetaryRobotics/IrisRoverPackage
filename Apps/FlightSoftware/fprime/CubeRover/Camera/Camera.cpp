// ======================================================================
// \title  CameraComponentImpl.cpp
// \author justin
// \brief  cpp file for Camera component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/Camera/Camera.hpp>
#include <CubeRover/NetworkManager/NetworkManager.hpp> // ! TODO: FIXME Hacky patch connection to steal NM data
#include "Fw/Types/BasicTypes.hpp"
#include "Include/FswPacket"    // PrimaryFlightController/FlightMCU
#include <cstring>
#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

#include "gio.h"
#include "spi.h"
#include "lin.h"

extern CubeRover::NetworkManagerComponentImpl networkManager; // ! TODO: FIXME Hacky patch connection to steal NM data

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CameraComponentImpl ::
#if FW_OBJECT_NAMES == 1
    CameraComponentImpl(
        const char *const compName
    ) :
      CameraComponentBase(compName)
#else
    CameraComponentImpl(void)
#endif
    ,
    Camera::CameraDownlinkManager(), m_cameraTask(this)
  {
      
  }

  void CameraComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    CameraComponentBase::init(instance);
    m_numComponentImgsReq = 0;
    m_numGroundImgsReq = 0;
    m_imagesSent = 0;
    m_bytesSent = 0;

    // Initialize the Camera Task:
    ::Os::Task::TaskStatus cameraTaskStat = m_cameraTask.startTask(CAMERA_TASK_PRIORITY,
                                                                   CAMERA_TASK_STACK_SIZE,
                                                                   CAMERA_TASK_CPU_AFFINITY);
    // Assert that this will always be started successfully. If it isn't, we're screwed.
    configASSERT(cameraTaskStat == Os::Task::TASK_OK);

  }

  CameraComponentImpl ::
    ~CameraComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
      schedIn_handler(
          const NATIVE_INT_TYPE portNum,
          NATIVE_UINT_TYPE context)
  {
      // Check if we need to downlink an image row and can downlink an image row:
      bool can_downlink = networkManager.m_pRadioDriver->m_networkInterface.udpTxQueueRoom() > 0;
      Camera::DownlinkRequest req = this->protectedDownlinkRequest.getData();
      bool need_to_downlink = !req.done;
      if(can_downlink && need_to_downlink){
          watchDogInterface.debugPrintfToWatchdog("CAM: Downlinking . . .");
          // Take the mutex:
          this->protectedDownlinkBuffer.mutex.lock();
          // Downlink:
          downlinkImage(this->protectedDownlinkBuffer.pData, req.numBytesToDownlink, req.callbackId, req.captureTimeMs, req.downlinkLineNumber, req.totalDownlinkLineCount);
          // Give the mutex back:
          this->protectedDownlinkBuffer.mutex.unLock();
          // Flag that we're done:
          req.done = true;
          this->protectedDownlinkRequest.setData(req);
      }
  }


  void CameraComponentImpl ::
    takePicture_handler(
        const NATIVE_INT_TYPE portNum,
        U8 CameraNum,
        U16 CallbackId
    )
  {
    m_numComponentImgsReq++;
    // Build image request object with default values
    Camera::ImageRequest imgReq ={
          .cam=static_cast<Camera::CameraSelection>(CameraNum),
          .callbackId=CallbackId,
          .skipXPairs=0,
          .skipYPairs=0,
          .startXPairs=0,
          .startYPairs=0,
          .stopXPairs=IMAGE_WIDTH/2, // name mismatch one side uses stop, the other end. no time to fix now. not actually an issue
          .stopYPairs=IMAGE_HEIGHT/2
    };
    triggerImageCapture(imgReq);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
    Get_Status_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
  Take_Image_cmdHandler(
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
      )
  {
    m_numGroundImgsReq++;
    tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);
    // Build image request object:

    Camera::ImageRequest imgReq ={
          .cam=static_cast<Camera::CameraSelection>(camera_num),
          .callbackId=callback_id,
          .skipXPairs=skipXPairs,
          .skipYPairs=skipYPairs,
          .startXPairs=startXPairs,
          .startYPairs=startYPairs,
          .stopXPairs=endXPairs, // name mismatch one side uses stop, the other end. no time to fix now. not actually an issue
          .stopYPairs=endYPairs
    };

    triggerImageCapture(imgReq);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Error_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 action
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Configure_Camera0_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U64 config
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Camera0_Crop_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U64 config
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Configure_Camera1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U64 config
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Camera1_Crop_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U64 config
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Read_Image_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U16 callbackID
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Erase_Image_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Soft_Camera_Reset_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 cameraNum
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
    Image_Dump_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }
  
  // ----------------------------------------------------------------------
  // User Methods
  // ----------------------------------------------------------------------


  void CameraComponentImpl::triggerImageCapture(Camera::ImageRequest imgReq)
  {
//      tlmWrite_Cam_LatestCallbackId(callbackId);
      // Set the image request data:
      m_cameraTask.m_imageRequest.setData(imgReq);
      // Trigger the capture:
      if(m_cameraTask.m_cameraState.getState() == Camera::CameraState::IDLE){
          // Only move to taking a new image if it's not in idle:
          // Move to taking image by setting state to SETUP:
          m_cameraTask.m_cameraState.setState(Camera::CameraState::SETUP);
      }
      m_imagesSent++;
      tlmWrite_Cam_ImagesSent(m_imagesSent);

  }

  void CameraComponentImpl::downlinkImage(uint8_t *image, int size, uint16_t callbackId, uint32_t captureTime, uint16_t downlinkLineNumber, uint16_t totalDownlinkLineCount)
  {
      Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
      downlinkImage_out(0, callbackId, captureTime, downlinkLineNumber, totalDownlinkLineCount, fwBuffer);
      m_bytesSent += static_cast<U32>(size);
      tlmWrite_Cam_BytesSent(m_bytesSent);
  }



} // end namespace CubeRover
