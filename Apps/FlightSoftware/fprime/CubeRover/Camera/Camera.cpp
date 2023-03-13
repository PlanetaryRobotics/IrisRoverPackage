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
#include "Fw/Types/BasicTypes.hpp"
#include "Include/FswPacket"    // PrimaryFlightController/FlightMCU
#include <cstring>

#include "gio.h"
#include "spi.h"
#include "lin.h"

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
  {
      
  }

  void CameraComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    CameraComponentBase::init(instance);
//    m_fpgaFlash.setupDevice();
    m_numComponentImgsReq = 0;
    m_numGroundImgsReq = 0;
    m_imagesSent = 0;
    m_bytesSent = 0;
  }

  CameraComponentImpl ::
    ~CameraComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
    takePicture_handler(
        const NATIVE_INT_TYPE portNum,
        U8 CameraNum,
        U16 CallbackId
    )
  {
    m_numComponentImgsReq++;
    tlmWrite_Cam_ComponentImagesRequested(m_numComponentImgsReq);
    triggerImageCapture(CameraNum, CallbackId);
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
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 camera_num,
        U16 callback_id
    )
  {
    m_numGroundImgsReq++;
    tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);
    triggerImageCapture(camera_num, callback_id);
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


  void CameraComponentImpl::triggerImageCapture(uint8_t camera, uint16_t callbackId)
  {
      tlmWrite_Cam_LatestCallbackId(callbackId);
      uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());

#ifdef DUMMY_IMG_GRID

      int grid_x_spacing = IMAGE_WIDTH / DUMMY_IMG_GRID;
      int grid_y_spacing = IMAGE_HEIGHT / DUMMY_IMG_GRID;

      for (int y = 0; y < IMAGE_HEIGHT; y++) {
          for (int x = 0; x < IMAGE_WIDTH; x++) {
              // if camera == 0 then all black, else black and white grid, in theory...
              m_imageLineBuffer[x] = camera * 255 * (((x / grid_x_spacing) + (y / grid_y_spacing)) % 2);
              // Make it a gradient in both x and y for debugging:
              if(m_imageLineBuffer[x] == 0x00){
                  m_imageLineBuffer[x] += 255 * x / IMAGE_WIDTH / 3;
                  m_imageLineBuffer[x] += 255 * y / IMAGE_HEIGHT / 3;
              } else {
                  m_imageLineBuffer[x] -= 255 * x / IMAGE_WIDTH / 3;
                  m_imageLineBuffer[x] -= 255 * y / IMAGE_HEIGHT / 3;
              }
          }
          downlinkImage(m_imageLineBuffer, sizeof(m_imageLineBuffer), callbackId, createTime);
      }
#endif
      m_imagesSent++;
      tlmWrite_Cam_ImagesSent(m_imagesSent);

  }

  void CameraComponentImpl::downlinkImage(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime)
  {
      Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
      downlinkImage_out(0, callbackId, createTime, fwBuffer);
      m_bytesSent += static_cast<U32>(size);
      tlmWrite_Cam_BytesSent(m_bytesSent);
  }



} // end namespace CubeRover
