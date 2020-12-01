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

#define CAMERA_1_SELECT         0x0001
#define CAMERA_2_SELECT         0x0001

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
      // m_fpgaFlash.setupDevice();
  }

  void CameraComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    CameraComponentBase::init(instance);
  }

  CameraComponentImpl ::
    ~CameraComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraComponentImpl ::
    takeImage_handler(
        const NATIVE_INT_TYPE portNum,
        U8 CameraNum
    )
  {
    // TODO
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
        U8 camera_num
    )
  {
    // TODO
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
  
    void CameraComponentImpl::downsampleLine() {
        for(uint32_t x = 0; x < IMAGE_WIDTH/DOWNSAMPLING; x++) {
            imageLineBuffer[x] = imageLineBuffer[x*DOWNSAMPLING];
        }
    }
  
    void CameraComponentImpl::selectCamera(int camera) {
        // ASSERT camera == 0 or 1
        int cameraSelect = CAMERA_1_SELECT;
        gioSetBit(linPORT, 1, cameraSelect & 0x01);   // FIXME: Replace camera select with dynamic val
        
        // add small delays to make sure camera is selection is done
        for(int delay=0; delay<500; delay++) asm("  NOP");
    }
  
    // TODO: Implement dual queue image capture to allow for two threads to downlink an
    // image at once (ie navigation and science photo)
    void CameraComponentImpl::triggerImageCapture(uint16_t callbackId) {
        uint16_t spiTxCmd = 0xFF;
        spiDAT1_t fpgaDataConfig;
        
        S25fl512l::MemAlloc alloc;
        alloc.startAddress = 0;     // Uhhh. Should use S25fl512l alloc method
        alloc.reservedSize = 0;

        fpgaDataConfig.CS_HOLD = false;
        fpgaDataConfig.DFSEL = SPI_FMT_0;
        fpgaDataConfig.WDEL = false;
        fpgaDataConfig.CSNR = 0;

        gioSetBit(spiPORT1, 0, 0); // set CS LOW
        spiTransmitData(spiREG1, &fpgaDataConfig, 1, &spiTxCmd);    // send data
        gioSetBit(spiPORT1, 0, 1); // set CS HIGH
        
        while(gioGetBit(gioPORTB, 1));  // Wait until image capture complete
        uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
            
        for(int i = 0; i < IMAGE_HEIGHT; i++) {
#ifdef __USE_DUMMY_IMAGE__
            getLineDummyImage(i, imageLineBuffer);
#else
            m_fpgaFlash.readDataFromFlash(&alloc, 0, imageLineBuffer, sizeof(imageLineBuffer));
            alloc.startAddress = 6 * PAGE_SIZE * i; // jump to next available block
#endif
            // downsampleLine();
            downlinkImage(imageLineBuffer, sizeof(imageLineBuffer), callbackId, createTime);
        }
    }
    
    void CameraComponentImpl::downlinkImage(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime) {
        Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
        downlinkImage_out(0, callbackId, createTime, fwBuffer);
    }
    

} // end namespace CubeRover
