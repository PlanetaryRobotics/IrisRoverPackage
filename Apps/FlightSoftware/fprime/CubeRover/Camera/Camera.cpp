// ======================================================================
// \title  CameraComponentImpl.cpp
// \author Connor Colombo, Raewyn Duvall
// \brief  cpp file for Camera component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <CubeRover/Camera/Camera.hpp>
#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include <CubeRover/IMU/IMUComponent.hpp>
#include <GroundInterface/GroundInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Include/FswPacket" // PrimaryFlightController/FlightMCU
#include <cstring>

#include "gio.h"
#include "spi.h"
#include "lin.h"

extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;
extern CubeRover::IMUComponentImpl IMU;

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
    m_lastCameraSelected = 0xAA; // none yet
    m_lastCaptureTime = 0;       // nothing yet

    // Init deployment image settings as not waiting:
    m_deploymentImageSettings = {
        .waitingForTrigger = false,
        .startTimeMs = 0 //
    };
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
    // Capture Image:
    uint32_t createTime = takeImage(CameraNum, CallbackId);
    // Downlink it:
    sendImgFromFlash(createTime, 0, IMAGE_HEIGHT);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
      Get_Status_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // DEPRECATED.
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
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
    // Capture Image:
    uint32_t createTime = takeImage(camera_num, callback_id);
    // Downlink it:
    sendImgFromFlash(createTime, 0, IMAGE_HEIGHT);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  /* Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line). */
  void CameraComponentImpl::Take_Image_Section_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq,          /*!< The command sequence number*/
      U8 camera_num,
      U16 startLine,
      U16 endLine,
      U16 callback_id)
  {
    m_numGroundImgsReq++;
    tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);

    if (startLine > endLine)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if ((endLine - startLine) <= 1)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if (startLine > (IMAGE_HEIGHT - 1) || endLine > IMAGE_HEIGHT)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }

    // Capture Image:
    uint32_t createTime = takeImage(camera_num, callback_id);
    // Downlink it:
    sendImgFromFlash(createTime, startLine, endLine);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  /* Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line). Offers advanced capture and downlink settings. */
  void CameraComponentImpl::Take_Image_Section_Adv_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq,          /*!< The command sequence number*/
      U8 camera_num,
      U16 startLine,
      U16 endLine,
      U16 callback_id,
      bool eraseFirst,
      uint8_t n_bin,
      bool compressLine)
  {
    m_numGroundImgsReq++;
    tlmWrite_Cam_CommandImagesRequested(m_numGroundImgsReq);

    if (startLine > endLine)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if ((endLine - startLine) <= 1)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if (startLine > (IMAGE_HEIGHT - 1) || endLine > IMAGE_HEIGHT)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }

    // Capture Image:
    uint32_t createTime = takeImage(camera_num, callback_id, eraseFirst);
    // Downlink it:
    sendImgFromFlash(createTime, startLine, endLine, n_bin, compressLine);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl::Read_Image_Section_Adv_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq,          /*!< The command sequence number*/
      U16 startLine,
      U16 endLine,
      uint8_t n_bin,
      bool compressLine)
  {
    // Validate args:
    if (startLine > endLine)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if ((endLine - startLine) <= 1)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }
    if (startLine > (IMAGE_HEIGHT - 1) || endLine > IMAGE_HEIGHT)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
    }

    // Downlink it:
    sendImgFromFlash(m_lastCaptureTime, startLine, endLine, n_bin, compressLine);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  /* Trigger Image Capture but don't downlink it (that can be done later with: `Read_Image_Section_Adv`). */
  void CameraComponentImpl::Capture_Image_Only_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq,          /*!< The command sequence number*/
      U8 camera_num,
      U16 callback_id,
      bool eraseFirst)
  {
    // Capture Image:
    takeImage(camera_num, callback_id, eraseFirst);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void Capture_Deployment_Image_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq,          /*!< The command sequence number*/
      // Capture Settings:
      U8 camera_num,
      U16 callback_id,
      bool eraseFirst,
      // Timeout Settings:
      U32 timeoutMs,
      bool triggerOnTimeout,
      // Trigger Settings:
      bool conditionModeAll,
      // IMU XAcc Settings:
      bool imuXAcc_on,
      bool imuXAcc_triggerMode,
      I16 imuXAcc_min,
      I16 imuXAcc_max,
      // IMU YAcc Settings:
      bool imuYAcc_on,
      bool imuYAcc_triggerMode,
      I16 imuYAcc_min,
      I16 imuYAcc_max,
      // IMU ZAcc Settings:
      bool imuZAcc_on,
      bool imuZAcc_triggerMode,
      I16 imuZAcc_min,
      I16 imuZAcc_max,
      // WDI 28V Settings:
      bool wdi28V_on,
      bool wdi28V_triggerMode,
      U16 wdi28V_min,
      U16 wdi28V_max //
  )
  {
    // Store settings:
    // Capture Settings:
    m_deploymentImageSettings.camera_num = camera_num;
    m_deploymentImageSettings.callback_id = callback_id;
    m_deploymentImageSettings.eraseFirst = eraseFirst;
    // Timeout Settings:
    m_deploymentImageSettings.timeoutMs = timeoutMs;
    m_deploymentImageSettings.triggerOnTimeout = triggerOnTimeout;
    // Trigger Settings:
    m_deploymentImageSettings.conditionModeAll = conditionModeAll;
    // IMU XAcc Settings:
    m_deploymentImageSettings.imuXAcc_on = imuXAcc_on;
    m_deploymentImageSettings.imuXAcc_triggerMode = imuXAcc_triggerMode;
    m_deploymentImageSettings.imuXAcc_min = imuXAcc_min;
    m_deploymentImageSettings.imuXAcc_max = imuXAcc_max;
    // IMU YAcc Settings:
    m_deploymentImageSettings.imuYAcc_on = imuYAcc_on;
    m_deploymentImageSettings.imuYAcc_triggerMode = imuYAcc_triggerMode;
    m_deploymentImageSettings.imuYAcc_min = imuYAcc_min;
    m_deploymentImageSettings.imuYAcc_max = imuYAcc_max;
    // IMU ZAcc Settings:
    m_deploymentImageSettings.imuZAcc_on = imuZAcc_on;
    m_deploymentImageSettings.imuZAcc_triggerMode = imuZAcc_triggerMode;
    m_deploymentImageSettings.imuZAcc_min = imuZAcc_min;
    m_deploymentImageSettings.imuZAcc_max = imuZAcc_max;
    // WDI 28V Settings:
    m_deploymentImageSettings.wdi28V_on = wdi28V_on;
    m_deploymentImageSettings.wdi28V_triggerMode = wdi28V_triggerMode;
    m_deploymentImageSettings.wdi28V_min = wdi28V_min;
    m_deploymentImageSettings.wdi28V_max = wdi28V_max;

    // Start listening for a trigger:
    m_deploymentImageSettings.startTimeMs = static_cast<uint32_t>(getTime().get_time_ms());
    m_deploymentImageSettings.waitingForTrigger = true; // set the flag last

    // Acknowledge Listen:
    log_ACTIVITY_HI_Camera_DeploymentImageCaptureWaiting(
        camera_num,
        callback_id,
        eraseFirst,
        // Timeout Settings:
        timeoutMs,
        triggerOnTimeout,
        // Trigger Settings:
        conditionModeAll,
        // IMU XAcc Settings:
        imuXAcc_on,
        imuXAcc_triggerMode,
        imuXAcc_min,
        imuXAcc_max,
        // IMU YAcc Settings:
        imuYAcc_on,
        imuYAcc_triggerMode,
        imuYAcc_min,
        imuYAcc_max,
        // IMU ZAcc Settings:
        imuZAcc_on,
        imuZAcc_triggerMode,
        imuZAcc_min,
        imuZAcc_max,
        // WDI 28V Settings:
        wdi28V_on,
        wdi28V_triggerMode,
        wdi28V_min,
        wdi28V_max //
    );

    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Error_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 action)
  {
    // DEPRECATED.
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Configure_Camera0_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // DEPRECATED. (configs now downlinked as command args)
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Camera0_Crop_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // DEPRECATED. Use section commands.
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Configure_Camera1_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // DEPRECATED. (configs now downlinked as command args)
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Camera1_Crop_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U64 config)
  {
    // DEPRECATED. Use section commands.
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Erase_Flash_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 numSectors)
  {
    eraseFpgaFlash(numSectors);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void CameraComponentImpl ::
      Soft_Camera_Reset_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq,
          U8 cameraNum)
  {
    //  DEPRECATED. Command WD directly to power off or reset FPGA.
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
  }

  void CameraComponentImpl ::
      Image_Dump_cmdHandler(
          const FwOpcodeType opCode,
          const U32 cmdSeq)
  {
    // Use capture time from last downlink:
    sendImgFromFlash(m_lastCaptureTime, 0, IMAGE_HEIGHT);
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
  // Triggers the capture of an image. Doesn't actually downlink it (in case we want to do that later).
  // Returns the capture time.
  uint32_t CameraComponentImpl::takeImage(uint8_t camera, uint16_t callbackId, bool eraseFirst)
  {
    // Set the camera and callback IDs
    m_cameraSelect = camera;
    m_lastCallbackId = callbackId;
    tlmWrite_Cam_LatestCallbackId(callbackId);

    // Take Real Image!

    // Automatically send the appropriate reset-specific command for camera
    // select:
    if (!m_cameraSelect)
    {
      watchDogInterface.Reset_Specific_Handler(WatchDogInterfaceComponentBase::reset_values_possible::FPGA_Cam_0);
    }
    else
    {
      watchDogInterface.Reset_Specific_Handler(WatchDogInterfaceComponentBase::reset_values_possible::FPGA_Cam_1);
    }
    // TODO: [CWC] This (^) is how resets were done before but shouldn't we be using `WatchdogResetRequest` for this? Investigate. It's done so infrequently and is now mutexed that it's probably okay.

    // set bit to control camera
    gioSetBit(linPORT, 1, m_cameraSelect & 0x01);
    m_lastCameraSelected = m_cameraSelect & 0x01;

    if (eraseFirst)
    {
      eraseFpgaFlash();
    }

    // add small delays to make sure camera is selection is done
    for (int delay = 0; delay < 500; delay++)
      asm("  NOP");

    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    m_lastCaptureTime = createTime;

    // capture image
    triggerImageCapture();
    // Wait until FPGA tells us its done:
    while (gioGetBit(gioPORTB, 1))
      ;

    log_ACTIVITY_HI_Camera_ImageCaptureComplete(
        GroundInterfaceComponentImpl::hashTime(createTime),
        m_lastCallbackId //
    );

    return createTime;
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
      alloc.reservedSize = sizeof(m_imageLine.buffer);

      eraseFpgaFlash();
    }

    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    m_lastCaptureTime = createTime;

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
          m_imageLine.buffer[x] = 255 * (((x / grid_x_spacing) + (y / grid_y_spacing)) % 2);
          // Make it a gradient in both x and y for debugging:
          if (m_imageLine.buffer[x] == 0x00)
          {
            m_imageLine.buffer[x] += 255 * x / DUMMY_IMAGE_WIDTH / 3;
            m_imageLine.buffer[x] += 255 * y / DUMMY_IMAGE_HEIGHT / 3;
          }
          else
          {
            m_imageLine.buffer[x] -= 255 * x / DUMMY_IMAGE_WIDTH / 3;
            m_imageLine.buffer[x] -= 255 * y / DUMMY_IMAGE_HEIGHT / 3;
          }
          break;
        case DummyImageType::SEQUENCE:
        default:
          // Make the sequence version where every 4B are an incrementing U32:
          m_imageLine.buffer[x] = sequenceCount.arr[sequenceByteCount % 4];
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
        m_fpgaFlash.writeDataToFlash(&alloc, 0, m_imageLine.buffer, DUMMY_IMAGE_WIDTH);
        alloc.startAddress += PAGE_SIZE * 6;
      }
      else
      {
        // send each line as it is created
        // Downlinking with formatting metadata:
        m_imageLine.formatting.binned = 0x00;
        m_imageLine.formatting.compressed = 0x00;
        downlinkImageLine(
            m_formattedImageLineBuffer, sizeof(ImageLineFormattingMetadata) + DUMMY_IMAGE_WIDTH,
            // Very late add: reserve top bit of callback Id for cameraNum:
            (m_lastCallbackId & 0x7FFF) | ((m_lastCameraSelected & 0x01) << 15),
            createTime, y, DUMMY_IMAGE_HEIGHT,
            (y == 0 || y == (DUMMY_IMAGE_HEIGHT - 1)) //
        );
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
    // Special command that's *not* a valid Flash command. FPGA spies on the
    // SPI and looks for this signal to take over control of the Flash chip.
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
  void CameraComponentImpl::eraseFpgaFlash(uint8_t numSectors)
  {
    for (int i = 0; i < numSectors; i++)
    {
      m_fpgaFlash.sectorErase(i);
    }
  }

  // SEND IMAGE FROM FLASH, reading from `startLine` to `endLine`
  // If n_bin > 2, bayer-preserving binning will be attempted.
  // If compressLine, lossless compression of binned line will be attempted.
  void CameraComponentImpl::sendImgFromFlash(
      uint32_t createTime,
      const uint32_t startLine,
      const uint32_t endLine,
      const uint8_t n_bin,
      const bool compressLine //
  )
  {
    static bool binningOccurred, compressionOccurred;
    if (startLine > endLine || (endLine - startLine) <= 1 || startLine > (IMAGE_HEIGHT - 1) || endLine > IMAGE_HEIGHT)
    {
      return;
    }

    S25fl512l::MemAlloc alloc;
    alloc.startAddress = 0;
    alloc.reservedSize = 0;

    for (int i = startLine; i < endLine; i++)
    {
      alloc.startAddress = IMAGE_PAGE_WIDTH * PAGE_SIZE * i; // set to correct block
      // Read out all PAGES in a line.
      // Since an image line doesn't full This should capture a region of blank
      // flash at the end of each line. We need this b/c FPGA's Flash FSM
      // sometimes places this skip in the wrong position.
      // RLE compression should make this extra 480B of data irrelevant since
      // it'll be compressed.
      m_fpgaFlash.readDataFromFlash(&alloc, 0, m_imageLine.buffer, sizeof(m_imageLine.buffer));

      // Optionally bin & compress data:
      uint16_t dataSize;
      if (n_bin > 2 || compressLine)
      {
        // Attempt to compress and bin:
        dataSize = ::IrisImage::compressAndBinLine(
            m_imageLine.buffer, sizeof(m_imageLine.buffer),
            n_bin, compressLine,
            &binningOccurred,
            &compressionOccurred //
        );
        m_imageLine.formatting.binned = binningOccurred ? 0xFF : 0x00;
        m_imageLine.formatting.compressed = compressionOccurred ? 0xFF : 0x00;
      }
      else
      {
        // Default:
        dataSize = sizeof(m_imageLine.buffer);
        m_imageLine.formatting.binned = 0x00;
        m_imageLine.formatting.compressed = 0x00;
      }

      // NOTE: Still using IMAGE_HEIGHT here (even if we're only sending a subset) because we need to know the total number of lines in the **FULL** FileGroup:
      downlinkImageLine(
          m_formattedImageLineBuffer, (sizeof(ImageLineFormattingMetadata) + dataSize),
          // Very late add: reserve top bit of callback Id for cameraNum:
          (m_lastCallbackId & 0x7FFF) | ((m_lastCameraSelected & 0x01) << 15),
          createTime, i, IMAGE_HEIGHT,
          (i == startLine || y == (endLine - 1)) //
      );
    }

    log_ACTIVITY_HI_Camera_ImageDownlinkComplete(
        GroundInterfaceComponentImpl::hashTime(createTime),
        m_lastCallbackId //
    );

    m_imagesSent++;
    tlmWrite_Cam_ImagesSent(m_imagesSent);
  }

  // DOWNLINK ONE ROW OF IMAGE
  void CameraComponentImpl::downlinkImageLine(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime, uint16_t lineIndex, uint16_t numLines, bool isFirstOrLastLineToDownlink)
  {
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
    downlinkImage_out(0, callbackId, createTime, lineIndex, numLines, isFirstOrLastLineToDownlink, fwBuffer);

    m_bytesSent += static_cast<U32>(size);
    tlmWrite_Cam_BytesSent(m_bytesSent);
  }

  void CameraComponentImpl::schedIn_handler(
      const NATIVE_INT_TYPE portNum,
      NATIVE_UINT_TYPE context)
  {
    static uint32_t time;
    static bool trigger;

    // Check for trigger conditions for deployment image:
    time = static_cast<uint32_t>(getTime().get_time_ms());

    trigger = false;
    // Check if we should still be looking for a deployment image trigger:
    if (m_deploymentImageSettings.waitingForTrigger && (time - m_deploymentImageSettings.startTimeMs) > m_deploymentImageSettings.timeoutMs)
    {
      // Time's up.
      // Stop looking:
      m_deploymentImageSettings.waitingForTrigger = false;
      // Trigger if we should we trigger on timeout:
      trigger = m_deploymentImageSettings.triggerOnTimeout;
    }

    // Check for a trigger:
    if (!trigger && deploymentImage_checkForTrigger(m_deploymentImageSettings))
    {
      trigger = true;
    }

    if (trigger)
    {
      // Deployment image trigger occurred!
      // Flag that we're no longer waiting to take an image (BEFORE capture):
      m_deploymentImageSettings.waitingForTrigger = false;
      // Set check flags to something more resilient to SEU:
      m_deploymentImageSettings.triggerOnTimeout = false;
      m_deploymentImageSettings.conditionModeAll = true;
      // Take the Image:
      time = takeImage(
          m_deploymentImageSettings.camera_num,
          m_deploymentImageSettings.callback_id,
          m_deploymentImageSettings.eraseFirst);
      log_ACTIVITY_HI_Camera_DeploymentImageCaptured(
          GroundInterfaceComponentImpl::hashTime(time),
          m_deploymentImageSettings.callback_id);
    }
  }

  // Returns whether it's time to trigger a deployment image based on the given settings.
  bool deploymentImage_checkForTrigger(DeploymentImageSettings &settings)
  {
    static int16_t accRawData[3];
    if (!settings.waitingForTrigger)
    {
      return false;
    }

    // Fetch latest IMU accelerations:
    IMU.getExtAccRaw(accRawData);

    // Check trigger conditions:
    // IMU XAcc Settings:
    bool imuXAcc_trig = deploymentImage_checkSensorForTrigger(
        accRawData[0],
        settings.imuXAcc_on,
        settings.imuXAcc_triggerMode,
        settings.imuXAcc_min,
        settings.imuXAcc_max //
    );
    // IMU YAcc Settings:
    bool imuYAcc_trig = deploymentImage_checkSensorForTrigger(
        accRawData[1],
        settings.imuYAcc_on,
        settings.imuYAcc_triggerMode,
        settings.imuYAcc_min,
        settings.imuYAcc_max //
    );
    // IMU ZAcc Settings:
    bool imuZAcc_trig = deploymentImage_checkSensorForTrigger(
        accRawData[2],
        settings.imuZAcc_on,
        settings.imuZAcc_triggerMode,
        settings.imuZAcc_min,
        settings.imuZAcc_max //
    );
    // WDI 28V Settings:
    bool wdi28V_trig = deploymentImage_checkSensorForTrigger(
        watchDogInterface.getExt28VRaw(),
        settings.wdi28V_on,
        settings.wdi28V_triggerMode,
        settings.wdi28V_min,
        settings.wdi28V_max //
    );

    // Combine triggers:
    if (settings.conditionModeAll)
    {
      // ALL must trigger:
      return (
          imuXAcc_trig &&
          imuYAcc_trig &&
          imuZAcc_trig &&
          wdi28V_trig);
    }
    else
    {
      // ANY can trigger:
      return (
          imuXAcc_trig ||
          imuYAcc_trig ||
          imuZAcc_trig ||
          wdi28V_trig);
    }
  }

  // Check the given sensor reading against the sensor's trigger conditions to
  // see if this deployment image trigger is true.
  template <typename T>
  bool deploymentImage_checkSensorForTrigger(
      T sensorReading,
      bool conditionOn,
      bool triggerMode,
      T windowMin,
      T windowMax)
  {
    if (conditionOn)
    {
      return false;
    }

    if (triggerMode)
    {
      // Trigger when inside the window:
      return (windowMin <= sensorReading) && (sensorReading < windowMax);
    }
    else
    {
      // Trigger when outside the window:
      return (windowMin > sensorReading) || (sensorReading >= windowMax);
    }
  }

} // end namespace CubeRover
