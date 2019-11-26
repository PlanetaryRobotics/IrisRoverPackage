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

    namespace S25fl064l{
      enum AddressLengthFormat{
        ADDRESS_LENGTH_3_BYTES  = 3,
        ADDRESS_LENGTH_4_BYTES  = 4
      };

       #define DEFAULT_DUMMY_CYCLES 8
       #define MAX_DUMMY_CYCLES     16

      struct StatusRegister1Bits{
        uint8_t wip:1;
        uint8_t wel:1;
        uint8_t bp:3;
        uint8_t tbProt:1;
        uint8_t sec:1;
        uint8_t srp0:1;
      };

      typedef union StatusRegister1{
        uint8_t all;
        StatusRegister1Bits bit;
      }StatusRegister1;

      struct StatusRegister2Bits{
        uint8_t ps:1;
        uint8_t es:1;
        uint8_t rsv:3;
        uint8_t p_err:1;
        uint8_t e_err:1;
        uint8_t rsv2:1;
      };

      typedef union StatusRegister2{
        uint8_t all;
        StatusRegister2Bits bit;
      }StatusRegister2;

      struct ConfigurationRegister1Bits{
        uint8_t srp1_d:1;
        uint8_t quad:1;
        uint8_t lb:4;
        uint8_t cmp:1;
        uint8_t sus:1;
      };

      typedef union ConfigurationRegister1{
        uint8_t all;
        ConfigurationRegister1Bits bit;
      }ConfigurationRegister1;

      struct ConfigurationRegister2Bits{
        uint8_t rsv:1;
        uint8_t adp:1;
        uint8_t wps:1;
        uint8_t qpi:1;
        uint8_t rsv2:1;
        uint8_t oi:2;
        uint8_t io3r:1;
      };

      typedef union ConfigurationRegister2{
        uint8_t all;
        ConfigurationRegister2Bits bit;
      }ConfigurationRegister2;

      struct ConfigurationRegister3Bits{
        uint8_t rl:4;
        uint8_t we:1;
        uint8_t wl:2;
        uint8_t rsv:1;
      };

      typedef union ConfigurationRegister3{
        uint8_t all;
        ConfigurationRegister3Bits bit;
      }ConfigurationRegister3;

      struct IrpRegisterBits{
        uint16_t permlb:1;
        uint16_t pslmlb:1;
        uint16_t pwdmlb:1;
        uint16_t rsv:1;
        uint16_t ibllbb:1;
        uint16_t rsv2:1;
        uint16_t secrrp:1;
        uint16_t rsv3:9;
      };

      typedef union IrpRegister{
        uint16_t all;
        IrpRegisterBits bit;
      }IrpRegister;

      typedef uint64_t PasswordRegister;

      struct ProtectionStatusRegisterBits{
        uint8_t nvclock:1;
        uint8_t rsv:5;
        uint8_t secrrp:1;
        uint8_t rsv2:1;
      };

      typedef union ProtectionStatusRegister{
        uint8_t all;
        uint8_t ProtectionStatusRegisterBits;
      }ProtectionStatusRegister;


      typedef uint8_t IblAccessRegister;

      struct PointerRegionProtectionRegisterBits{
        uint32_t rsv:9;
        uint32_t prptb:1;
        uint32_t prpen:1;
        uint32_t prpall:1;
        uint32_t rsv2:4;
        uint32_t prpad:7;
        uint32_t rsv3:9;
      };

      typedef union PointerRegionProtectionRegister{
        uint32_t all;
        PointerRegionProtectionRegisterBits bit;
      }PointerRegionProtectionRegister;

      typedef uint8_t NonVolatileDataLearningDataRegister;
      typedef uint8_t VolatileDataLearningDataRegister;
      typedef uint32_t Address;
      typedef uint16_t PageNumber;
      typedef uint16_t Block;
      typedef uint16_t HalfBlock;
      typedef uint16_t Sector;

      #define PAGE_SIZE         0x100     // 256B
      #define SECTOR_SIZE       0x1000    // 4KB
      #define HALF_BLOCK_SIZE   0x8000    // 32KB
      #define BLOCK_SIZE        0x10000   // 64KB

      #define MAX_MEMORY_ADDRESS    0x7FFFFF            // 64MB
      #define MAX_BLOCK_RANGE       128                 // There is 128 blocks (from block #0 to block #127)
      #define MAX_HALF_BLOCK_RANGE  MAX_BLOCK_RANGE*2 
      #define MAX_SECTOR_RANGE      MAX_BLOCK_RANGE*16  // There are 16 sectors per block (16*128 = 2048 sectors)

      #define ADDRESS_NOT_DEFINED   0xFFFFFFFF

      #define FLASH_MANUFACTURER_ID   0x01
      #define FLASH_DEVICE_ID_MSB     0x60
      #define FLASH_DEVICE_ID_LSB     0x17

      #define SIZE_NAVIGATION_IMAGE1  65536 // bytes
      #define SIZE_NAVIGATION_IMAGE2  65536 // bytes
      #define SIZE_SCIENCE_IMAGE      5000000 //bytes

      typedef struct MemAlloc{
        Address startAddress;
        uint32_t reservedSize;
      }MemAlloc;

      typedef enum FlashSpiCommands{
        // Read Device ID
        RDID =        0x9F,   // Read ID
        RSFDP =       0x5A,   // Read JEDEC Serial Flash Discoverable parameters
        RDQID =       0xAF,   // Read Quad ID
        RUID =        0x4B,   // Read Unique ID
        // Register access
        RDSR1 =       0x05,   // Read Status Register 1
        RDSR2 =       0x07,   // Read Status Register 2
        RDCR1 =       0x35,   // Read Control Register 1
        RDCR2 =       0x15,   // Read Control Register 2
        RDCR3 =       0x33,   // Read Control Register 3
        RDAR =        0x65,   // Rad  any register 
        WRR =         0x01,   // Write Register (Status -1 and configuration -1,2,3)
        WRDI =        0x04,   // Write Disable
        WREN =        0x06,   // Write enable for non volatile data change
        WRENV =       0x50,   // Write enable for volatile status and configuration registers
        WRAR =        0x71,   // Write any register
        CLSR =        0x30,   // Clear status register
        _4BEN =       0xB7,   // Enter 4 byte address mode
        _4BEX =       0xE9,   // Exit 4 byte address mode
        SBEL =        0x77,   // Set burst length
        QPIEN =       0x38,   // Enter QPI
        QPIEX =       0xF5,   // Exit QPI
        DLPRD =       0x41,   // Data learning pattern read
        PDLRNV =      0x43,   // Program NV data learning register
        WDLRV =       0x4A,   // Write volatile data learning center
        // Read flash array
        _4READ =      0x13,   // Read
        _4FAST_READ = 0x0C,   // Read
        FAST_READ =   0x0B,   // Fast read
        READ =        0x03,   // Fast read
        _4DOR =       0x3C,   // Dual output read
        _4QOR =       0x6C,   // Dual output read
        DOR =         0x3B,   // Dual output read
        QOR =         0x6B,   // Quad output read
        QIOR =        0xEB,   // Quad I/O read
        DIOR =        0xBB,   // Dual I/O read
        _4DIOR =      0xBC,   // Dual I/O read
        _4QIOR =      0xEC,   // Quad I/O read
        _4DDRQIOR =   0xEE,   // DDR Quad I/O read
        DDRQIOR =     0xED,   // DDR Quad I/O read
        // Program flash array
        PP =          0x02,   // Page program
        _4PP =        0x12,   // Page program
        QPP =         0x32,   // Quad page program
        _4QPP =       0x34,   // Quad page program
        // Erase flash array
        SE =          0x20,   // Sector erase
        _4SE =        0x21,   // Sector erase
        HBE =         0x52,   // Half-block erase
        _4HBE =       0x54,   // Half-block erase
        BE =          0xD8,   // Block erase
        _4BE =        0xDC,   // Block erase
        CE =          0x60,   // Chip erase
        // Erase / Program / Suspend / Resume
        EPS =         0x75,   // Erase / program suspend
        EPR =         0x7A,   // Erase / program resume
        // Individual and region protection
        SECRE =       0x44,   // Security region erase
        SECRP =       0x42,   // Security region program
        SECRR =       0x48,   // Security region read
        // Array protection
        IBLRD =       0x3D,   // IBL read
        _4IBLRD =     0xE0,   // IBL read
        IBL =         0x36,   // IBL lock
        _4IBL =       0xE1,   // IBL lock
        _4IBUL =      0xE2,   // IBL unblock
        IBUL =        0x39,   // IBL unblock
        GBL =         0x7E,   // Global IBL block
        GBUL =        0x98,   // Global IBL unblock
        SPRP =        0xFB,   // Set pointer region protection
        _4SPRP =      0xE3,   // Set pointer region protection
        // Reset
        RSTEN =       0x66,   // Software reset enable
        RST =         0x99,   // Software reset
        MBR =         0xFF,   // Mode bit reset
        // Deep power down
        DPD =         0xB9,   // Deep power down
        RES =         0xAB    // Release from deep power down / device id
      }FlashSpiCommands;
    };

    namespace CycloneFpga{

      typedef uint16_t ImageIndex;

      typedef enum CycloneFpgaRegister{
        STS       =    0x00,
        TAKE_PIC  =    0x01,
        CFG_CAM_1 =    0x02,
        CFG_CAM_2 =    0x03,
        ERROR =        0x04,
        CROP_CAM_1 =   0x05,
        CROP_CAM_2 =   0x06,
        JPEG_DAT =     0x07,
        IMG_STR =      0x08,
        IMG_SIZE =     0x09,
        ERASE =        0x0A
      }CycloneFpgaRegister;

      struct StatusRegisterBits{
        uint8_t wipImage1:1; // capture/compress in progress for image sensor #1
        uint8_t wipImage2:1; // capture/compress in progress for image sensor #2
        uint8_t eip:1;       // erase in progress
        uint8_t error:1;     // error bit
        uint8_t bip:1;       // boot in progress
        uint8_t rsv:3;       // reserved for future use
      };

      typedef union StatusRegister{
        uint8_t all;
        StatusRegisterBits bit;
      }StatusRegister;

      struct TakePictureRegisterBits{
        uint32_t cameraId:1;          // Camera selection 0b: camera 1, 1b: camera 2
        uint32_t imgStoreIndex:16;    // Storage index of the image
        uint32_t rsv:15;              // reserved for future use
      }TakePictureRegisterBits;

      typedef union TakePictureRegister{
        uint32_t all;
        TakePictureRegisterBits bit;
      }TakePictureRegister;

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

      struct CameraCropRegister{
        uint32_t upperLeftXValue;       // image crop upper left corner x-axis
        uint32_t upperLeftYValue;       // image crop upper left corner y-axis
        uint32_t lowerRightXValue;      // image crop lower right corner x-axis
        uint32_t lowerRightYValue;      // image crop lower right corner y-axis
      };

      struct JpegDataRegister{
        uint32_t imageIndex:16;         // image index
        uint32_t resetImagePointer:1;   // reset image pointer
        uint32_t rsv:15;
      };

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
      CAMERA_FAIL_SPI_WRITE = -11
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
      CameraError setup(spiBASE_t *flashSpi, spiBASE_t *fpgaSpi);

    private:
      //Functions specific to interface to FPGA
      CameraError setupFPGAInterface();

      //Functions specific to interface to external flash memory
      CameraError setupExternalFlash();
      CameraError flashSpiReadData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                uint8_t *rxData,
                                const uint16_t dataReadLength,
                                CameraInterface::S25fl064l::Address address = ADDRESS_NOT_DEFINED); 

      CameraError flashSpiWriteData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                uint8_t *txData = NULL,
                                const uint16_t dataWriteLength = 0,
                                CameraInterface::S25fl064l::Address address = ADDRESS_NOT_DEFINED); 
      uint16_t getAddressLengthByte(const CameraInterface::S25fl064l::FlashSpiCommands cmd);
      CameraError allocateFlashMemory(CameraInterface::S25fl064l::MemAlloc *alloc,
                                      const uint32_t size);
      CameraError sectorErase(const CameraInterface::S25fl064l::Sector sector);
      CameraError halfBlockErase(const CameraInterface::S25fl064l::HalfBlock halfBlock);
      CameraError blockErase(const CameraInterface::S25fl064l::Block block);
      CameraError chipErase();
      CameraError resetDevice();
      CameraError programEraseResume();
      CameraError programEraseSuspend();
      CameraError pageProgram(CameraInterface::S25fl064l::Address address,
                              uint8_t *txData,
                              const uint16_t size);

      CameraError writeDataToFlash(CameraInterface::S25fl064l::MemAlloc *alloc,
                                   const uint32_t offset,
                                   uint8_t *data,
                                   const uint16_t dataSize);
      CameraError readDataFromFlash(CameraInterface::S25fl064l::MemAlloc *alloc,
                                    const uint32_t offset,
                                    uint8_t *data,
                                    const uint16_t dataSize);
      uint8_t getReadDummyCycles(const CameraInterface::S25fl064l::FlashSpiCommands cmd);

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
      spiDAT1_t m_flashDataConfig;
      spiDAT1_t m_fpgaXfaceConfig;
      spiBASE_t *m_flashSpi;
      spiBASE_t *m_fpgaSpi;
      bool m_setup;
      uint32_t m_readLatencyCycles;
      uint16_t m_spiRxBuff[SPI_RX_BUFFER_MAX_LENGTH];
      uint16_t m_spiTxBuff[SPI_TX_BUFFER_MAX_LENGTH];
      uint8_t m_writeScratchpad[PAGE_SIZE];
      uint8_t m_sectorBackup[SECTOR_SIZE];
      uint8_t m_dummyBuffer[MAX_DUMMY_CYCLES];
      CameraInterface::S25fl064l::AddressLengthFormat m_addressLengthFormat;

      //memory pointer that keeps track of the last allocated memory pointer
      CameraInterface::S25fl064l::Address m_memAllocPointer;

      CameraInterface::S25fl064l::MemAlloc m_memAllocNavCam1;
      CameraInterface::S25fl064l::MemAlloc m_memAllocNavCam2;
      CameraInterface::S25fl064l::MemAlloc m_memAllocScienceImg;
    };

} // end namespace CubeRover

#endif
