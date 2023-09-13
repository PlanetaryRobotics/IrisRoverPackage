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
#include "Include/FswPacket" // PrimaryFlightController/FlightMCU
#include <cstring>

#include "gio.h"
#include "spi.h"
#include "lin.h"

namespace CubeRover
{

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CameraComponentImpl ::
#if FW_OBJECT_NAMES == 1
      CameraComponentImpl(
          const char *const compName) : CameraComponentBase(compName)
#else
      CameraComponentImpl(void)
#endif
  {
  }

  void CameraComponentImpl ::
      init(
          const NATIVE_INT_TYPE instance)
  {
    CameraComponentBase::init(instance);
    m_fpgaFlash.setupDevice();
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
          U16 CallbackId)
  {
    m_numComponentImgsReq++;
    tlmWrite_Cam_ComponentImagesRequested(m_numComponentImgsReq);
    takeImage(CameraNum, CallbackId, 0, IMAGE_HEIGHT);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
      Get_Status_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Take_Image_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 camera_num,
          U16 callback_id)
  {
    m_numGroundImgsReq++;
    tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);
    takeImage(camera_num, callback_id, 0, IMAGE_HEIGHT);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  /* Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line). */
  void CameraComponentImpl::Take_Image_Section_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq, /*!< The command sequence number*/
      U8 camera_num,
      U16 startLine,
      U16 endLine,
      U16 callback_id
  ){
      m_numGroundImgsReq++;
      tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);

      if(startLine > endLine){
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
      }
      if((endLine-startLine) <= 1){
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
      }
      if(startLine > (IMAGE_HEIGHT-1) || endLine > IMAGE_HEIGHT){
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
      }

      takeImage(camera_num, callback_id, startLine, endLine);
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Error_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 action)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Configure_Camera0_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Camera0_Crop_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Configure_Camera1_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Camera1_Crop_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Read_Image_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U16 callbackID)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Erase_Image_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Soft_Camera_Reset_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 cameraNum)
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Image_Dump_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // Capture time isn't stored, so just use now as the createTime:
    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    sendImgFromFlash(createTime, 0, IMAGE_HEIGHT);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Downlink_Grid_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq, /*!< The command sequence number*/
          const bool viaFlash)
  {
    generateDummyImage(viaFlash, DummyImageType::GRID);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Downlink_Test_Sequence_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq, /*!< The command sequence number*/
          const bool viaFlash)
  {
    generateDummyImage(viaFlash, DummyImageType::SEQUENCE);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // User Methods
  // ----------------------------------------------------------------------

  // TAKE IMAGE
  void CameraComponentImpl::takeImage(uint8_t camera, uint16_t callbackId, const uint32_t startLine, const uint32_t endLine)
  {
    // Set the camera and callback IDs
    m_cameraSelect = camera;
    m_lastCallbackId = callbackId;
    tlmWrite_Cam_LatestCallbackId(callbackId);

    // Take Real Image!

    // set bit to control camera
    gioSetBit(linPORT, 1, m_cameraSelect & 0x01);

    eraseFpgaFlash();

    // add small delays to make sure camera is selection is done
    for (int delay = 0; delay < 500; delay++)
      asm("  NOP");

    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());

    // capture image
    triggerImageCapture();
    while (gioGetBit(gioPORTB, 1))
      ;

    // send image from flash
    sendImgFromFlash(createTime, startLine, endLine);
  }

  // CREATE AND SEND DUMMY IMAGE
  void CameraComponentImpl::generateDummyImage(bool viaFlash, DummyImageType type)
  {
    int grid_x_spacing = DUMMY_IMAGE_WIDTH / DUMMY_IMG_GRID_n;
    int grid_y_spacing = DUMMY_IMAGE_HEIGHT / DUMMY_IMG_GRID_n;

    S25fl512l::MemAlloc alloc;
    if (viaFlash)
    {
      // Prep Flash before writing each line
      alloc.startAddress = 0;
      alloc.reservedSize = sizeof(m_imageLineBuffer);

      eraseFpgaFlash();
    }

    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());

    union
    {
      uint32_t val;
      uint8_t arr[4];
    } sequenceCount;
    sequenceCount.val = 0;
    uint32_t sequenceByteCount = 0;

    for (int y = 0; y < DUMMY_IMAGE_HEIGHT; y++)
    {
      // Make a line:
      for (int x = 0; x < DUMMY_IMAGE_WIDTH; x++)
      {
        switch (type)
        {
        case DummyImageType::GRID:
          // Make the grid version:
          // if camera == 0 then all black, else black and white grid, in theory...
          m_imageLineBuffer[x] = 255 * (((x / grid_x_spacing) + (y / grid_y_spacing)) % 2);
          // Make it a gradient in both x and y for debugging:
          if (m_imageLineBuffer[x] == 0x00)
          {
            m_imageLineBuffer[x] += 255 * x / DUMMY_IMAGE_WIDTH / 3;
            m_imageLineBuffer[x] += 255 * y / DUMMY_IMAGE_HEIGHT / 3;
          }
          else
          {
            m_imageLineBuffer[x] -= 255 * x / DUMMY_IMAGE_WIDTH / 3;
            m_imageLineBuffer[x] -= 255 * y / DUMMY_IMAGE_HEIGHT / 3;
          }
          break;
        case DummyImageType::SEQUENCE:
        default:
          // Make the sequence version where every 4B are an incrementing U32:
          m_imageLineBuffer[x] = sequenceCount.arr[sequenceByteCount % 4];
          if (sequenceByteCount % 4 == 3)
          {
            // Inc. to next value once all bytes in this value have been added:
            sequenceCount.val++;
          }
          sequenceByteCount++;
        }
      } // end of line
      if (viaFlash)
      {
        // write each line to flash
        m_fpgaFlash.writeDataToFlash(&alloc, 0, m_imageLineBuffer, sizeof(m_imageLineBuffer));
        alloc.startAddress += PAGE_SIZE * 6;
      }
      else
      {
        // send each line as it is created
        downlinkImageLine(m_imageLineBuffer, sizeof(m_imageLineBuffer), m_lastCallbackId, createTime, y, DUMMY_IMAGE_HEIGHT);
      }
    }

    if (viaFlash)
    {
      // read and send whole image from flash
      sendImgFromFlash(createTime, 0, IMAGE_HEIGHT);
    }

    // Finished sending Dummy Image
    m_imagesSent++;
    tlmWrite_Cam_ImagesSent(m_imagesSent);
  }

  // TRIGGER IMAGE CAPTURE ON CAMERA
  void CameraComponentImpl::triggerImageCapture()
  {
    uint16_t spiTxCmd = 0xFF;
    spiDAT1_t g_fpgaDataConfig;

    g_fpgaDataConfig.CS_HOLD = false;
    g_fpgaDataConfig.DFSEL = SPI_FMT_0;
    g_fpgaDataConfig.WDEL = false;
    g_fpgaDataConfig.CSNR = 0;

    gioSetBit(spiPORT1, 0, 0); // set CS LOW

    // send data
    spiTransmitData(spiREG1, &g_fpgaDataConfig, 1, &spiTxCmd);

    gioSetBit(spiPORT1, 0, 1); // set CS HIGH
  }

  // ERASE FLASH
  void CameraComponentImpl::eraseFpgaFlash(void)
  {
    for (int i = 0; i < 40; i++)
    {
      m_fpgaFlash.sectorErase(i);
    }
  }

  // SEND IMAGE FROM FLASH, reading from `startLine` to `endLine`
  void CameraComponentImpl::sendImgFromFlash(
      uint32_t createTime,
      const uint32_t startLine,
      const uint32_t endLine)
  {
      if(startLine > endLine || (endLine-startLine) <= 1 || startLine > (IMAGE_HEIGHT-1) || endLine > IMAGE_HEIGHT){
          return;
      }

    S25fl512l::MemAlloc alloc;
    alloc.startAddress = 0;
    alloc.reservedSize = 0;

    for (int i = startLine; i < endLine; i++)
    {
      alloc.startAddress = 6 * PAGE_SIZE * i; // set to correct block
      m_fpgaFlash.readDataFromFlash(&alloc, 0, m_imageLineBuffer, sizeof(m_imageLineBuffer));

      // NOTE: Still using IMAGE_HEIGHT here (even if we're only sending a subset) because we need to know the total number of lines in the **FULL** FileGroup:
      downlinkImageLine(m_imageLineBuffer, sizeof(m_imageLineBuffer), m_lastCallbackId, createTime, i, IMAGE_HEIGHT);
    }

    m_imagesSent++;
    tlmWrite_Cam_ImagesSent(m_imagesSent);
  }

  // DOWNLINK ONE ROW OF IMAGE
  void CameraComponentImpl::downlinkImageLine(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime, uint16_t lineIndex, uint16_t numLines)
  {
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
    downlinkImage_out(0, callbackId, createTime, lineIndex, numLines, fwBuffer);

    m_bytesSent += static_cast<U32>(size);
    tlmWrite_Cam_BytesSent(m_bytesSent);
  }

} // end namespace CubeRover
