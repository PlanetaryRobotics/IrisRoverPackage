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
        U16 CallbackId
    )
  {
    m_numComponentImgsReq++;
    tlmWrite_Cam_ComponentImagesRequested(m_numComponentImgsReq);
    triggerImageCapture(CameraNum, CallbackId, portNum + 1);
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
    triggerImageCapture(camera_num, callback_id, 0);
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
  
    void CameraComponentImpl::downsampleLine(int bufferNum) {
        for(uint32_t x = 0; x < IMAGE_WIDTH/DOWNSAMPLING; x++) {
            m_imageLineBuffer[bufferNum][x] = m_imageLineBuffer[bufferNum][x*DOWNSAMPLING];
        }
    }
  
    void CameraComponentImpl::selectCamera(int cameraSelect) {
        // ASSERT camera == 0 or 1
        gioSetBit(linPORT, 1, cameraSelect & 0x01);
        
        // add small delays to make sure camera is selection is done
        for(int delay=0; delay<500; delay++) asm("  NOP");
    }
  
    // TODO: Implement dual queue image capture to allow for two threads to downlink an
    // image at once (ie navigation and science photo)
    void CameraComponentImpl::triggerImageCapture(uint8_t camera, uint16_t callbackId, int bufferNum) {
        uint16_t spiTxCmd = 0xFF;
        spiDAT1_t fpgaDataConfig;
        
        S25fl512l::MemAlloc alloc;
        alloc.startAddress = 0;     // Uhhh. Should use S25fl512l alloc method
        alloc.reservedSize = 0;

        fpgaDataConfig.CS_HOLD = false;
        fpgaDataConfig.DFSEL = SPI_FMT_0;
        fpgaDataConfig.WDEL = false;
        fpgaDataConfig.CSNR = 0;
        
        selectCamera(static_cast<int>(camera));

        /* TODO: MUTEX CRITICAL SECTION START */
        gioSetBit(spiPORT1, 0, 0); // set CS LOW
        spiTransmitData(spiREG1, &fpgaDataConfig, 1, &spiTxCmd);    // send data
        gioSetBit(spiPORT1, 0, 1); // set CS HIGH
        
        tlmWrite_Cam_LatestCallbackId(callbackId);
        while(gioGetBit(gioPORTB, 1));  // Wait until image capture complete FIXME: This could loop forever :(
        /* MUTEX CRITICAL SECTION END */
        uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
            
        // TODO: Operator should be able to specify DOWNSAMPLING (but for testing smaller images are faster
        for(int i = 0; i < IMAGE_HEIGHT; i+=DOWNSAMPLING) {
#ifdef __USE_DUMMY_IMAGE__
            getLineDummyImage(i, m_imageLineBuffer[bufferNum]);
#else
            /* TODO: MUTEX CRITICAL SECTION START */
            m_fpgaFlash.readDataFromFlash(&alloc, 0, m_imageLineBuffer[bufferNum], sizeof(m_imageLineBuffer[bufferNum])); // TODO: It would be nice to DMA this
            /* MUTEX CRITICAL SECTION END */
            alloc.startAddress = 6 * PAGE_SIZE * i; // jump to next available block
#endif
            downsampleLine(bufferNum);
            downlinkImage(m_imageLineBuffer[bufferNum], sizeof(m_imageLineBuffer[bufferNum]), callbackId, createTime);
        }
        m_imagesSent++;
        tlmWrite_Cam_ImagesSent(m_imagesSent);
    }
    
    void CameraComponentImpl::downlinkImage(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime) {
        Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(image), size);
        downlinkImage_out(0, callbackId, createTime, fwBuffer);
        m_bytesSent += static_cast<U32>(size);
        tlmWrite_Cam_BytesSent(m_bytesSent);
    }
    

} // end namespace CubeRover
