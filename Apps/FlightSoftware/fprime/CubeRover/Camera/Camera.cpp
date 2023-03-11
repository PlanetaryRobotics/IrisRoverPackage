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

    g_cameraSelect = 0;
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

      void CameraComponentImpl::eraseFpgaFlash(){
          for(int i=0; i< 40; i++){
              m_fpgaFlash.sectorErase(i);
          }
      }

    void CameraComponentImpl::downsampleLine() {
        for(uint32_t x = 0; x < IMAGE_WIDTH/DOWNSAMPLING; x++) {
            m_imageLineBuffer[x] = m_imageLineBuffer[x*DOWNSAMPLING];
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
    void CameraComponentImpl::triggerImageCapture(uint8_t camera, uint16_t callbackId) {

#ifdef __USE_DUMMY_IMAGE__
        TEST__getAndTransmitPicture();
        return;
#endif

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

        // RAD TODO - added
        eraseFpgaFlash();

        gioSetBit(spiPORT1, 0, 0); // set CS LOW
        spiTransmitData(spiREG1, &fpgaDataConfig, 1, &spiTxCmd);    // send data
        gioSetBit(spiPORT1, 0, 1); // set CS HIGH
        
        tlmWrite_Cam_LatestCallbackId(callbackId);
        while(gioGetBit(gioPORTB, 1));  // Wait until image capture complete FIXME: This could loop forever :(
        uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
            
        // TODO: Operator should be able to specify DOWNSAMPLING (but for testing smaller images are faster
        for(int i = 0; i < IMAGE_HEIGHT; i+=DOWNSAMPLING) {
//#ifdef __USE_DUMMY_IMAGE__
//            getLineDummyImage(i, m_imageLineBuffer);
//#else
            m_fpgaFlash.readDataFromFlash(&alloc, 0, m_imageLineBuffer, sizeof(m_imageLineBuffer));
            alloc.startAddress = 6 * PAGE_SIZE * i; // jump to next available block
//#endif
            downsampleLine();
            downlinkImage(m_imageLineBuffer, sizeof(m_imageLineBuffer), callbackId, createTime);
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
    

    // ---------------------------------------------------------------------------------
    // RAD TODO START: THIS ENTIRE BLOCK IS FOR DUMMY IMAGES KEPT IN .__UNUSED__/FUNCTIONS.CPP

  #ifdef __USE_DUMMY_IMAGE__

    // TODO - Make this a grid
      void CameraComponentImpl::TEST__generateDummyImage(){
        for(int x=0; x < IMAGE_WIDTH; x++){
            for(int y=0; y < IMAGE_HEIGHT; y++){
                g_dummyImage[x+y*IMAGE_WIDTH] = x%255 + y%255;
            }
        }
     }

#ifdef __USE_FPGA_FLASH__

     void CameraComponentImpl::TEST__fpgaTriggerCameraCapture(){
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

     void CameraComponentImpl::TEST__dummyFlashWrite(){
        uint8_t buffer[IMAGE_WIDTH];
        for(int i=0;i<sizeof(buffer);i++){
            buffer[i] = i%128;
        }
        S25fl512l::MemAlloc alloc;
        alloc.startAddress = 0;
        alloc.reservedSize = sizeof(buffer);

        for(int i=0; i<IMAGE_HEIGHT; i++){
            m_fpgaFlash.writeDataToFlash(&alloc, 0, buffer, sizeof(buffer));
            alloc.startAddress += PAGE_SIZE * 6;
        }
     }

     void CameraComponentImpl::TEST__eraseFpgaFlash(){
        for(int i=0; i< 40; i++){
            m_fpgaFlash.sectorErase(i);
        }
     }

#endif

     void CameraComponentImpl::TEST__getLineDummyImage(int line, uint8_t *dstBuff){
        memcpy(dstBuff, g_dummyImage + line*IMAGE_WIDTH, IMAGE_WIDTH);
    }

     void CameraComponentImpl::TEST__downsampleLine(){
        for(uint32_t x=0; x<IMAGE_WIDTH/DOWNSAMPLING; x++){
            m_imageLineBuffer[x] = m_imageLineBuffer[x*DOWNSAMPLING];
        }
     }

      void CameraComponentImpl::TEST__getAndTransmitPicture(){
   //      imageTransferStatus_t status = BEGIN_NEW_TRANSFER;
        S25fl512l::MemAlloc alloc;
        alloc.startAddress = 0;
        alloc.reservedSize = 0;

        // set bit to control camera
        gioSetBit(linPORT, 1, g_cameraSelect & 0x01);

        TEST__eraseFpgaFlash();

        TEST__dummyFlashWrite();

        // add small delays to make sure camera is selection is done
        for(int delay=0; delay<500; delay++) asm("  NOP");

        TEST__fpgaTriggerCameraCapture();
        while(gioGetBit(gioPORTB, 1));

        for(int i=0;i<IMAGE_HEIGHT; i++){
   // #ifdef __USE_DUMMY_IMAGE__
   //         TEST__getLineDummyImage(i, m_imageLineBuffer);
   // #else
            m_fpgaFlash.readDataFromFlash(&alloc, 0, m_imageLineBuffer, sizeof(m_imageLineBuffer));
            alloc.startAddress = 6* PAGE_SIZE * i; // jump to next available block
   // #endif
            TEST__downsampleLine();
   //          status = (i == IMAGE_HEIGHT - 1) ? LAST_TRANSFER : CONTINUE;
            // sendCameraDataOverWifi(g_imageLineBuffer, sizeof(g_imageLineBuffer) / DOWNSAMPLING, status);
        }
    }

  #endif

    // RAD TODO END: THIS ENTIRE BLOCK IS FOR DUMMY IMAGES KEPT IN .__UNUSED__/FUNCTIONS.CPP
    // ---------------------------------------------------------------------------------


} // end namespace CubeRover
