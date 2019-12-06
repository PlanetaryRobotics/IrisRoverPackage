// ======================================================================
// \title  CameraInterfaceComponentImpl.cpp
// \author cedric
// \brief  cpp file for CameraInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/CameraInterface/CameraInterfaceComponentImpl.hpp>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "Fw/Types/BasicTypes.hpp"
#include "CubeRoverConfig.hpp"
#include "gio.h"
#include "spi.h"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CameraInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    CameraInterfaceComponentImpl(
        const char *const compName
    ) :
      CameraInterfaceComponentBase(compName)
#else
    CameraInterfaceComponentImpl(void)
#endif
  {
    m_setup = false;
    m_fpgaSpi = NULL;
  }


  /**
   * @brief      Initialize the camera interface
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void CameraInterfaceComponentImpl :: init( const NATIVE_INT_TYPE queueDepth,
                                             const NATIVE_INT_TYPE instance){
    CameraInterfaceComponentBase::init(queueDepth, instance);
  }


  /**
   * @brief      Destroys the object.
   */
  CameraInterfaceComponentImpl ::
    ~CameraInterfaceComponentImpl(void)
  {

  }


  /**
   * @brief      Setup the interface to the external flash memory
   *
   * @param      flashSpi  The flash spi
   * @param      fpgaSpi   The fpga spi
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: setup(spiBASE_t *fpgaSpi){
    CameraError err = CAMERA_NO_ERROR;

    if(fpgaSpi == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    m_fpgaSpi = fpgaSpi;

    if(err != CAMERA_NO_ERROR)
      return err;

    m_setup = true;

    return err;
  }

  /**
   * @brief      Setup the FPGA interface
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: setupFPGAInterface(){

      return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Gets the image size.
   *
   * @param[in]  index    The index
   * @param      imgSize  The image size
   *
   * @return     The camera error.
   */
  CameraError CameraInterfaceComponentImpl :: getImageSize(const CameraInterface::CycloneFpga::ImageIndex index,
                                                           CameraInterface::CycloneFpga::ImageSize *imgSize){
    if(imgSize == NULL || index == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Read the image size for a given index
    return fpgaSpiRead(CameraInterface::CycloneFpga::SpiRegister::IMG_SIZE,   /* spi register */
                      (uint8_t *)imgSize,                                     /* data to write to */
                      sizeof(CameraInterface::CycloneFpga::ImageSize),        /* data size */
                      (uint8_t *)index);                                      /* argument to the read command */
  }

  /**
   * @brief      Gets the number image stored.
   *
   * @param      nbImageStored  The number of image stored
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: getNumberImageStored(uint32_t *nbImageStored){
    if(nbImageStored == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Read the number of image stored
    return fpgaSpiRead(CameraInterface::CycloneFpga::SpiRegister::IMG_STR, /* spi register */
                       (uint8_t *)&nbImageStored,                      /* data to write to */
                       sizeof(uint32_t));                              /* size of the data to be read */
  } 

  /**
   * @brief      Gets the image.
   *
   * @param[in]  index           The index
   * @param[in]  newRead         The new read
   * @param[in]  imgData         The image data
   * @param[in]  nbOfByteToRead  The number of of byte to read
   *
   * @return     The image.
   */
  CameraError CameraInterfaceComponentImpl :: getImage(const CameraInterface::CycloneFpga::ImageIndex index,
                                                       const bool newRead,
                                                       const uint8_t *imgData,
                                                       const uint32_t nbOfByteToRead){
    CameraError err;
    CameraInterface::CycloneFpga::JpegDataRegister jpegReg;

    // Clear register
    jpegReg.all = 0;

    // Set the index we want to read
    jpegReg.bit.imageIndex = index;

    // If marked as new read, reset image pointer
    jpegReg.bit.resetImagePointer = (newRead) ? 1 : 0;

    err = fpgaSpiRead(CameraInterface::CycloneFpga::SpiRegister::JPEG_DAT, /* spi register */
                      (uint8_t *)imgData,                                  /* data to write to */
                      nbOfByteToRead,                                      /* data size */
                      (uint8_t *)&jpegReg.all);                            /* argument to the read command */
    return err;
  }

  /**
   * @brief      Gets the error.
   *
   * @param[in]  reg   The error register to read 
   *
   * @return     The camera error.
   */
  CameraError CameraInterfaceComponentImpl :: getError(CameraInterface::CycloneFpga::ErrorRegister *reg){
    CameraError err;
    CameraInterface::CycloneFpga::ErrorRegister clrFaultMsk;

    if(reg == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    clrFaultMsk.all = 0xFFFF; // 1b: read the bit, 0b: clear the fault. In that instance we only want to read.

    err = fpgaSpiRead(CameraInterface::CycloneFpga::SpiRegister::ERROR_FPGA, /* spi register */
                      (uint8_t *)reg->all,                                   /* data to write to */
                      sizeof(reg->all),                                      /* size of data to read */
                      (uint8_t *) &clrFaultMsk.all);                         /* argument to the read command */

    return err;
  }

  /**
   * @brief      Clear the error(s) from the error register
   *
   * @param      msk   The bit mask to control what bits needs to be erased
   *                   setting a bit to 0 clear the error, setting a bit to 1
   *                   has no effect
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: clearError(CameraInterface::CycloneFpga::ErrorRegister *msk){
    CameraError err;

    if(msk == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    err = fpgaSpiWrite(CameraInterface::CycloneFpga::SpiRegister::ERROR_FPGA, /* spi register */
                       (uint8_t *) msk->all);                                 /* data to write to fpga */

    return err;
  }

  /**
   * @brief      Take a picture from the front or rear camera and store it with
   *             an index
   *
   * @param[in]  id        The identifier
   * @param[in]  imgIndex  The image index
   *
   * @return     The camera error code 
   */
  CameraError CameraInterfaceComponentImpl :: takePicture(const CameraInterface::CycloneFpga::CameraId id, 
                                                          const CameraInterface::CycloneFpga::ImageIndex imgIndex){
    CameraInterface::CycloneFpga::ControlRegister ctlReg;
    CameraInterface::CycloneFpga::StatusRegister statusReg;
    CameraError err;
    uint32_t tries = __INT_MAX;

    // Populate the control register to be sent to FPGA
    ctlReg.all = 0;
    ctlReg.bit.cameraId = id;
    ctlReg.bit.imgStoreIndex = imgIndex;

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(CameraInterface::CycloneFpga::SpiRegister::CTL,  /* spi register to write */
                      (uint8_t *)&ctlReg.all);                          /* data to write to fpga */

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Read the status register to check when the image capture is completed.
    while(tries > 0){
      err = fpgaSpiRead(CameraInterface::CycloneFpga::SpiRegister::STS, /* spi register to read */
                        &statusReg.all,                                 /* data to write to */
                        sizeof(statusReg.all));                         /* size of the data */

      if(err != CAMERA_NO_ERROR){
        return err;
      }   

      // Check if image processing is in progress
      if(statusReg.bit.wipCamera1 || statusReg.bit.wipCamera2){
        tries--;
        continue;
      }  

      if(statusReg.bit.error){
        return CAMERA_FAIL_IMAGE_CAPTURE;
      }
    }

    return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Sets the image cropping.
   *
   * @param[in]  id         The new value
   * @param[in]  upperLeft  The top left
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl ::
    setImageCropping(const CameraInterface::CycloneFpga::CameraId id,
                     const CameraInterface::CycloneFpga::PixelCoordinate upperLeft,
                     const CameraInterface::CycloneFpga::PixelCoordinate lowerRight){

    CameraInterface::CycloneFpga::CameraCropRegister *cropReg;
    CameraInterface::CycloneFpga::SpiRegister reg;
    CameraError err = CAMERA_NO_ERROR;

    // Set local configuration register pointer to corresponding camera configuration register
    cropReg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? &m_cropCam1 : &m_cropCam2;

    // Populate data to be sent to FPGA
    cropReg->upperLeft.x = upperLeft.x;
    cropReg->upperLeft.y = upperLeft.y;
    cropReg->lowerRight.x = lowerRight.x;
    cropReg->lowerRight.y = lowerRight.y;

    // Point to correct spi register corresponding to camera selected
    reg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_1:
                                                          CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_2;

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(reg,                   /* spi register */
                      (uint8_t *)cropReg);    /* data to write to fpga */

    return err;    
  }

  /**
   * @brief      Sets the shutter speed.
   *
   * @param[in]  id    The new value
   * @param[in]  shut  The shut
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl ::
    setShutterSpeed(const CameraInterface::CycloneFpga::CameraId id,
                    const CameraInterface::CycloneFpga::ShutterSpeed shut){

    CameraInterface::CycloneFpga::ConfigurationRegister *cfgReg;
    CameraInterface::CycloneFpga::SpiRegister reg;
    CameraError err = CAMERA_NO_ERROR;

    // Set local configuration register pointer to corresponding camera
    // configuration register
    cfgReg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? &m_cfgCam1 : &m_cfgCam2;

    // Point to correct spi register corresponding to camera selected
    reg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1 :
                                                          CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2;

    // Update color mode for the camera
    cfgReg->bit.shutterSpeed = shut & 0x3FF; // just take the 12 LSB

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(reg,                       /* spi register */
                      (uint8_t *)cfgReg->all);    /* data to write to fpga */

    return err;    
  }


  /**
   * @brief      Sets the camera exposure value.
   *
   * @param[in]  id    The new value
   * @param[in]  exp   The new value
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl ::
    setExposureValue(const CameraInterface::CycloneFpga::CameraId id,
                     const CameraInterface::CycloneFpga::CameraExposure exp){

    CameraInterface::CycloneFpga::ConfigurationRegister *cfgReg;
    CameraInterface::CycloneFpga::SpiRegister reg;
    CameraError err = CAMERA_NO_ERROR;

    // Set local configuration register pointer to corresponding camera
    // configuration register
    cfgReg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? &m_cfgCam1 : &m_cfgCam2;

    // Point to correct spi register corresponding to camera selected
    reg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1 :
                                                          CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2;

    // Update color mode for the camera
    cfgReg->bit.exposure = exp & 0x3FF; // just take the 12 LSB

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(reg,                         /* spi register */
                      (uint8_t *)cfgReg->all);      /* data to write to fpga */

    return err;    
  }

  /**
   * @brief      Sets the image color mode.
   *
   * @param[in]  id    The new value
   * @param[in]  rgb   The new value
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl ::
    setImageColorMode(const CameraInterface::CycloneFpga::CameraId id,
                      const CameraInterface::CycloneFpga::ImageColorType rgb){

    CameraInterface::CycloneFpga::ConfigurationRegister *cfgReg;
    CameraInterface::CycloneFpga::SpiRegister reg;
    CameraError err = CAMERA_NO_ERROR;

    // Set local configuration register pointer to corresponding camera
    // configuration register
    cfgReg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? &m_cfgCam1 : &m_cfgCam2;

    // Point to correct spi register corresponding to camera selected
    reg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1 :
                                                          CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2;

    // Update color mode for the camera
    cfgReg->bit.rgb = rgb;

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(reg,                           /* spi register */
                       (uint8_t *)cfgReg->all);       /* data to write to fpga */

    return err;
  }

  /**
   * @brief      Sets the image compression level.
   *
   * @param[in]  id    The new value
   * @param[in]  comp  The component
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl ::
    setImageCompressionLevel(const CameraInterface::CycloneFpga::CameraId id,
                             const CameraInterface::CycloneFpga::CompressionLevel comp){

    CameraInterface::CycloneFpga::ConfigurationRegister *cfgReg;
    CameraInterface::CycloneFpga::SpiRegister reg;
    CameraError err = CAMERA_NO_ERROR;

    // Set local configuration register pointer to corresponding camera
    // configuration register
    cfgReg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? &m_cfgCam1 : &m_cfgCam2;

    // Point to correct spi register corresponding to camera selected
    reg = (id == CameraInterface::CycloneFpga::CAM_ONE) ? CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1 :
                                                          CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2;

    // Update compression level
    cfgReg->bit.compressionLevel = comp;

    // Send a command to capture a picture from a camera at a given index.
    err = fpgaSpiWrite(reg,                               /* spi register */
                      (uint8_t *)cfgReg->all);            /* data to write to fpga */

    return err;
  }
                                                              
  /**
   * @brief      Send data over SPI to the FPGA
   *
   * @param[in]  reg     The register
   * @param      txData  The transmit data
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: fpgaSpiWrite(const CameraInterface::CycloneFpga::SpiRegister reg,
                                                           uint8_t *txData){
    uint16_t totalBytesToTransmit;
    uint16_t sizeOfTxReply;
    uint16_t sizeOfTxData;
    uint32_t i;


    m_spiTxBuff[0] = (uint8_t) reg & 0x7F; // mask MSB that contains Write/Read bit

    // Calculate how many bytes to transmit, this is pre-defined based on the
    // type command to send
    sizeOfTxData = getCmdArgLengthByte(reg);
    totalBytesToTransmit = sizeOfTxData + 1 /*command*/;

    // Check that the total number of bytes to transmit fit the transmit buffer
    if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH){
      return CAMERA_WRONG_DATA_SIZE;
    }

    // Check that pointer is valid when we need to send data
    if(sizeOfTxData > 0 && txData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Copy the data to transmit into the transmit buffer
    for(i=0; i< totalBytesToTransmit-1 /* don't include header byte */; i++){
      m_spiTxBuff[i+1] = *txData;
      txData++;
    }

    // Set CS low
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 0);

    // Transmit data
    spiTransmitData(m_fpgaSpi,                  /* hercules spi register pointer */
                    &m_fpgaDataConfig,          /* spi data configuration handler */
                    totalBytesToTransmit,       /* total byte to transmit to slave */
                    (uint16_t *)&m_spiTxBuff);  /* size of packet to transmit to slave */

    // Receive slave copy of write commands 
    sizeOfTxReply = getSizeOfAckData(reg);

    if(sizeOfTxReply > 0){
      spiReceiveData(m_fpgaSpi,                 /* hercules spi register pointer */
                     &m_fpgaDataConfig,         /* spi data configuration handler */
                     sizeOfTxReply,             /* total byte to transmit to slave */
                     (uint16_t *)&m_spiRxBuff); /* size of packet to transmit to slave */
    }

    // Set CS high
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 1);

    // Write commands are acknowledged by FPGA by echoing data received
    // Check that the command acknowledge match the command sent
    for(i=0; i<sizeOfTxData; i++){
      if(m_spiTxBuff[i+1 /*offset by 1 to exclude command*/] != m_spiRxBuff[i]){
        return CAMERA_FAIL_SPI_WRITE;
      }
    }

    return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Read some data over SPI
   *
   * @param[in]  reg           The register
   * @param      rxData        The receive data
   * @param[in]  sizeOfRxData  The size of the receive data
   * @param      argData       The transmit data (required only when an arg is
   *                           passed to read some data)
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: fpgaSpiRead(const CameraInterface::CycloneFpga::SpiRegister reg,
                                                          uint8_t *rxData,
                                                          const uint16_t sizeOfRxData,
                                                          uint8_t *argData){
    uint16_t totalBytesToTransmit;
    uint16_t argDataSize;
    uint32_t i;

    if(rxData == NULL && sizeOfRxData > 0){
      return CAMERA_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) reg & 0x7F;    // MSB is reserved for read/write command, clear it.
    m_spiTxBuff[0] |= 0x80;                   // set bit for a read command

    totalBytesToTransmit = 1 /*command*/;

    // This function reports a size > 0 when an argument is required to read
    // some data. This number can change based on the command sent
    argDataSize = getCmdArgLengthByte(reg);

    if(argDataSize > 0){
      if(argData == NULL){ 
        return CAMERA_UNEXPECTED_ERROR;
      }

      totalBytesToTransmit += argDataSize;
      for(i=0; i<argDataSize; i++){
        m_spiTxBuff[i+1 /*command*/] = argData[i];
      }
    }

    // Check that the total number of bytes to transmit fit the transmit buffer
    if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH){
      return CAMERA_WRONG_DATA_SIZE;
    }

    // Check that pointer is valid when we need to send data
    if(sizeOfRxData > 0 && rxData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Set CS low
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 0);

    // Transmit data
    spiTransmitData(m_fpgaSpi,                  /* hercules spi register */
                   &m_fpgaDataConfig,           /* hercules spi data configuration handler */
                   totalBytesToTransmit,        /* number of bytes to transmit to slave */
                   (uint16_t *)&m_spiTxBuff);   /* buffer to transmit */

    spiReceiveData(m_fpgaSpi,                   /* hercules spi register */
                   &m_fpgaDataConfig,           /* hercules spi data configuration handler */
                   sizeOfRxData,                /* number of bytes to transmit to slave */
                   (uint16_t *)&m_spiRxBuff);   /* buffer to transmit */

    // Set CS high
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 1);

    return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Gets the data length byte for a SDI data transfer
   *
   * @param[in]  reg   The register
   *
   * @return     The data length byte.
   */
  uint16_t CameraInterfaceComponentImpl :: getCmdArgLengthByte(const CameraInterface::CycloneFpga::SpiRegister reg){
    switch(reg){
      case CameraInterface::CycloneFpga::SpiRegister::STS:
      case CameraInterface::CycloneFpga::SpiRegister::IMG_STR:
        return 0;
      case CameraInterface::CycloneFpga::SpiRegister::IMG_SIZE:
      case CameraInterface::CycloneFpga::SpiRegister::ERASE:
      case CameraInterface::CycloneFpga::SpiRegister::ERROR_FPGA:
        return 2;
      case CameraInterface::CycloneFpga::SpiRegister::CTL:
      case CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1:
      case CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2:
      case CameraInterface::CycloneFpga::SpiRegister::JPEG_DAT:
        return 4;
      case CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_1:
      case CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_2:
        return 16;
      default:
        return 0;
    }
  }

  /**
   * @brief      Gets the size of acknowledge data for write commands
   *
   * @param[in]  reg   The register
   *
   * @return     The size of acknowledge data.
   */
  uint16_t CameraInterfaceComponentImpl :: getSizeOfAckData(const CameraInterface::CycloneFpga::SpiRegister reg){
    switch(reg){
      case CameraInterface::CycloneFpga::SpiRegister::CTL:
      case CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_1:
      case CameraInterface::CycloneFpga::SpiRegister::CFG_CAM_2:
      case CameraInterface::CycloneFpga::SpiRegister::ERROR_FPGA:
        return 4; // size of the data acknowledgment. See software design spec
                  // document.
      case CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_1:
      case CameraInterface::CycloneFpga::SpiRegister::CROP_CAM_2:
        return 16; // size of the data acknowledgment. See software design spec
                   // document.
      case CameraInterface::CycloneFpga::SpiRegister::STS:
      case CameraInterface::CycloneFpga::SpiRegister::IMG_STR:
      case CameraInterface::CycloneFpga::SpiRegister::IMG_SIZE:
      case CameraInterface::CycloneFpga::SpiRegister::JPEG_DAT:
      case CameraInterface::CycloneFpga::SpiRegister::ERASE:
      default:
        return 0; // size of the data acknowledgment. See software design spec
                  // document.
    }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraInterfaceComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CameraInterfaceComponentImpl ::
    CAM_CaptureImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraInterfaceComponentImpl ::
    CAM_FetchImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
