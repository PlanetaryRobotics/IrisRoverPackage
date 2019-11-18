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

      typedef union IrpRegister{
        uint16_t all;
        IrpRegisterBits bit;
      }IrpRegister;

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

      typedef uint8_t* Address;
      
      typedef uint16_t PageNumber;

      #define PAGE_SIZE         256

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
      typedef enum CycloneFpgaRegister{
      }CycloneFpgaRegister;
    }
  }

  typedef enum CameraError{
      CAMERA_NO_ERROR = 0,
      CAMERA_UNEXPECTED_ERROR = -1,
      CAMERA_WRONG_DATA_SIZE = -2
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

      CameraError setup(spiBASE_t *spi);
      CameraError setupExternalFlash(spiBASE_t *spi);
      CameraError setupFPGAInterface(spiBASE_t *spi);
      CameraError flashSpiReadData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                uint16_t *rxData,
                                const uint16_t dataReadLength,
                                CameraInterface::S25fl064l::Address address = NULL); 

      CameraError flashSpiWriteData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                uint16_t *txData = NULL, 
                                const uint16_t dataWriteLength = 0,
                                CameraInterface::S25fl064l::Address address = NULL); 
    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

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
      spiBASE_t *m_spi;
      bool m_setup;
      uint32_t m_readLatencyCycles;
      uint16_t m_spiRxBuff[SPI_RX_BUFFER_MAX_LENGTH];
      uint16_t m_spiTxBuff[SPI_TX_BUFFER_MAX_LENGTH];
      uint16_t getAddressLengthByte(const CameraInterface::S25fl064l::FlashSpiCommands cmd);
      CameraInterface::S25fl064l::AddressLengthFormat m_addressLengthFormat;
    };

} // end namespace CubeRover

#endif
