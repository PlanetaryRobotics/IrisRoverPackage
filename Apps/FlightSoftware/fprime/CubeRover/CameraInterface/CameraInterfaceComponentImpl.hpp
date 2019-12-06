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

#include <stdlib.h>
#include "CubeRover/CameraInterface/CameraInterfaceComponentAc.hpp"
#include "spi.h"

namespace CubeRover {
  namespace CameraInterface{
    #define SPI_RX_BUFFER_MAX_LENGTH 261    // Worst case: 256 data payload bytes + 1 byte command + 4 bytes addressing
    #define SPI_TX_BUFFER_MAX_LENGTH 261

    namespace CycloneFpga{

      typedef uint16_t ImageIndex;
      typedef uint16_t CameraExposure;
      typedef uint32_t ImageSize;
      typedef uint16_t ShutterSpeed;

      typedef enum SpiRegister{
        STS       =    0x00,
        CTL       =    0x01,
        CFG_CAM_1 =    0x02,
        CFG_CAM_2 =    0x03,
        ERROR_FPGA =   0x04,
        CROP_CAM_1 =   0x05,
        CROP_CAM_2 =   0x06,
        JPEG_DAT =     0x07,
        IMG_STR =      0x08,
        IMG_SIZE =     0x09,
        ERASE =        0x0A
      }SpiRegister;

      typedef enum CameraId{
        CAM_ONE =   0,
        CAM_TWO =   1
      }CameraId;

      typedef enum ImageColorType{
        GRAYSCALE_IMG = 0,
        COLOR_IMG = 1
      }ImageColorType;

      typedef enum CompressionLevel{
        NO_COMPRESSION = 0,
        COMPRESSION_25_PERCENT = 1,
        COMPRESSION_50_PERCENT = 2,
        COMPRESSION_75_PERCENT = 3
      }CompressionLevel;

      struct StatusRegisterBits{
        uint8_t wipCamera1:1; // capture/compress in progress for image sensor #1
        uint8_t wipCamera2:1; // capture/compress in progress for image sensor #2
        uint8_t eip:1;       // erase in progress
        uint8_t error:1;     // error bit
        uint8_t bip:1;       // boot in progress
        uint8_t rsv:3;       // reserved for future use
      };

      typedef union StatusRegister{
        uint8_t all;
        StatusRegisterBits bit;
      }StatusRegister;

      struct ControlRegisterBits{
        uint32_t cameraId:1;          // Camera selection 0b: camera 1, 1b: camera 2
        uint32_t imgStoreIndex:16;    // Storage index of the image
        uint32_t rsv:15;              // reserved for future use
      };

      typedef union ControlRegister{
        uint32_t all;
        ControlRegisterBits bit;
      }ControlRegister;

      struct ConfigurationRegisterBits{
        uint32_t compressionLevel:2;    //Compression level 00b:0%, 01b:25%, 10b:50%, 11b:75%
        uint32_t rgb:1;                 // 0b: grayscale, 1b color
        uint32_t exposure:12;           // exposure time
        uint32_t shutterSpeed:12;       // shutter speed
        uint32_t rsv:5;                 // reserved for future use
      };

      typedef union ConfigurationRegister{
        uint32_t all;
        ConfigurationRegisterBits bit;
      }ConfigurationRegister;

      struct ErrorRegisterBits{
        uint32_t imageIndex:16;         // image index 
        uint32_t cameraId:1;            // camera id 0b : camera 1, 1b camera 2
        uint32_t cameraTimeout:10;      // timeout occured during the camera image acquisition / configuration
        uint32_t imageCaptureFail:1;    // image capture failed
        uint32_t cameraNotDetected:1;   // camera not detected
        uint32_t writeFailure:1;        // image write to FPGA ext. memory failed
        uint32_t readFailure:1;         // image read from FPGA ext. memory failed
        uint32_t eraseFailure:1;        // image erase from FPGA ext. memory failed
        uint32_t rsv:9;                 // reserved for future use
      };

      typedef union ErrorRegister{
        uint32_t all;
        ErrorRegisterBits bit;
      }ErrorRegister;

      typedef struct PixelCoordinate{
        uint32_t x;
        uint32_t y;
      }PixelCoordinate;

      struct CameraCropRegister{
        PixelCoordinate upperLeft;      
        PixelCoordinate lowerRight;         
      };

      struct JpegDataRegisterBits{
        uint32_t imageIndex:16;         // image index
        uint32_t resetImagePointer:1;   // reset image pointer
        uint32_t rsv:15;
      };

      typedef union JpegDataRegister{
          uint32_t all;
          JpegDataRegisterBits bit;
      }JpegDataRegister;

      typedef uint16_t ImageSizeRegister;
      typedef uint32_t EraseRegister;

    }
  }

  typedef enum CameraError{
      CAMERA_NO_ERROR = 0,
      CAMERA_UNEXPECTED_ERROR = -1,
      CAMERA_WRONG_DATA_SIZE = -2,
      CAMERA_FAIL_MEM_ALLOCATION = -3,
      CAMERA_FAIL_PAGE_PROGRAM = -4,
      CAMERA_FAIL_SECTOR_ERASE = -5,
      CAMERA_FAIL_WRITE_DATA_FLASH = -6,
      CAMERA_FAIL_ERASE_CHIP = -7,
      CAMERA_FAIL_HALF_BLOCK_ERASE = -8,
      CAMERA_FAIL_BLOCK_ERASE = -9,
      CAMERA_INCORRECT_FLASH_MEMORY = -10,
      CAMERA_FAIL_SPI_WRITE = -11,
      CAMERA_FAIL_IMAGE_CAPTURE = -12
  }CameraError;

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

      // Setup interface to flash memory and fpga
      CameraError setup(spiBASE_t *fpgaSpi);

    private:
      //Functions specific to interface to FPGA
      CameraError setupFPGAInterface();
      uint16_t getCmdArgLengthByte(const CameraInterface::CycloneFpga::SpiRegister reg);
      uint16_t getSizeOfAckData(const CameraInterface::CycloneFpga::SpiRegister reg);
      CameraError getImageSize(const CameraInterface::CycloneFpga::ImageIndex index,
                               CameraInterface::CycloneFpga::ImageSize *size);
      CameraError getNumberImageStored(uint32_t *nbImageStored);
      CameraError getImage(const CameraInterface::CycloneFpga::ImageIndex index,
                           const bool newRead,
                           const uint8_t *imgData,
                           const uint32_t nbOfByteToRead);
      CameraError takePicture(const CameraInterface::CycloneFpga::CameraId id, 
                              const CameraInterface::CycloneFpga::ImageIndex imgIndex);
      CameraError setImageCropping(const CameraInterface::CycloneFpga::CameraId id,
                                   const CameraInterface::CycloneFpga::PixelCoordinate upperLeft,
                                   const CameraInterface::CycloneFpga::PixelCoordinate lowerRight);
      CameraError setShutterSpeed(const CameraInterface::CycloneFpga::CameraId id,
                                  const CameraInterface::CycloneFpga::ShutterSpeed shut);
      CameraError setExposureValue(const CameraInterface::CycloneFpga::CameraId id,
                                   const CameraInterface::CycloneFpga::CameraExposure exp);
      CameraError setImageColorMode(const CameraInterface::CycloneFpga::CameraId id,
                                    const CameraInterface::CycloneFpga::ImageColorType rgb);
      CameraError setImageCompressionLevel(const CameraInterface::CycloneFpga::CameraId id,
                                           const CameraInterface::CycloneFpga::CompressionLevel comp);
      CameraError getError(CameraInterface::CycloneFpga::ErrorRegister *reg);
      CameraError clearError(CameraInterface::CycloneFpga::ErrorRegister *msk);
      CameraError fpgaSpiWrite(const CameraInterface::CycloneFpga::SpiRegister reg,
                               uint8_t *txData);
      CameraError fpgaSpiRead(const CameraInterface::CycloneFpga::SpiRegister reg,
                              uint8_t *rxData,
                              const uint16_t sizeOfRxData,
                              uint8_t *argData = NULL);

    private:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    private:

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
      spiDAT1_t m_fpgaDataConfig;
      spiBASE_t *m_fpgaSpi;
      bool m_setup;
      uint16_t m_spiRxBuff[SPI_RX_BUFFER_MAX_LENGTH];
      uint16_t m_spiTxBuff[SPI_TX_BUFFER_MAX_LENGTH];

      CameraInterface::CycloneFpga::CameraCropRegister m_cropCam1;
      CameraInterface::CycloneFpga::CameraCropRegister m_cropCam2;
      CameraInterface::CycloneFpga::ConfigurationRegister m_cfgCam1;
      CameraInterface::CycloneFpga::ConfigurationRegister m_cfgCam2;
    };

} // end namespace CubeRover

#endif
