#ifndef __S25fl512l_HPP__
#define __S25fl512l_HPP__

#include <spi.h>
#include <string.h>
#include <gio.h>
#include <math.h>

class S25fl512l{
  #define SPI_RX_BUFFER_MAX_LENGTH (512+5)    // Worst case: 512 data payload bytes + 1 byte command + 4 bytes addressing
  #define SPI_TX_BUFFER_MAX_LENGTH (512+5)

  #define SPI_REG spiREG1
  #define CS_SPI_PORT spiPORT1
  #define CS_SPI_BIT  0


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
    uint8_t e_err:1;
    uint8_t p_err:1;
    uint8_t srwd:1;
  };

  typedef union StatusRegister1{
    uint8_t all;
    StatusRegister1Bits bit;
  }StatusRegister1;

  struct StatusRegister2Bits{
    uint8_t ps:1;
    uint8_t es:1;
    uint8_t rvd:6;
  };

  typedef union StatusRegister2{
    uint8_t all;
    StatusRegister2Bits bit;
  }StatusRegister2;

  struct ConfigurationRegister1Bits{
   uint8_t freeze:1;
   uint8_t quad:1;
   uint8_t rsv2:1;
   uint8_t bpnv:1;
   uint8_t rsvd1:1;
   uint8_t tbprot:1;
   uint8_t lc:2;
  };

  typedef union ConfigurationRegister1{
    uint8_t all;
    ConfigurationRegister1Bits bit;
  }ConfigurationRegister1;

  struct  BankAddressRegisterBits{
      uint8_t rsvd1:1;
      uint8_t ba25:1;
      uint8_t rsvd2:5;
      uint8_t extAdd:1;
  };

  typedef union BankAddressRegister{
      uint8_t all;
      BankAddressRegisterBits bit;
  }BankAddressRegister;

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
  typedef uint8_t NonVolatileDataLearningDataRegister;
  typedef uint8_t VolatileDataLearningDataRegister;
  typedef uint32_t Address;
  typedef uint16_t Page;
  typedef uint16_t Sector;

  #define PAGE_SIZE               0x200     // 512B
  #define SECTOR_SIZE             0x40000    //256KB

  #define MAX_MEMORY_ADDRESS      0x3FFFFFF          // 512Mb
  #define MAX_SECTOR_RANGE        256


  #define ADDRESS_NOT_DEFINED     0xFFFFFFFF

  #define FLASH_MANUFACTURER_ID   0x01
  #define FLASH_DEVICE_ID         0x19

  typedef enum FlashSpiCommands{
    // Read Device ID
    RDID =        0x9F,   // Read ID
    REMS =        0x90,   // Read ID - REMS
    RSFDP =       0x5A,   // Read JEDEC Serial Flash Discoverable parameters
    RDQID =       0xAF,   // Read Quad ID
    RUID =        0x4B,   // Read Unique ID
    // Register access
    RDSR1 =       0x05,   // Read Status Register 1
    RDSR2 =       0x07,   // Read Status Register 2
    RDCR1 =       0x35,   // Read Control Register 1
    RDAR =        0x65,   // Read  any register
    ECCRD =       0x18,   // Read ECC 4 byte addr
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
    SE =          0xD8,   // Sector erase
    _4SE =        0xDC,   // Sector erase
    BE =          0x60,   // Bulk erase
    _4BE =        0xDC,   // Block erase
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
    RES =         0xAB,    // Release from deep power down / device id
    // Bank register configuration
    BRWR =        0x17   // Bank register write
  }FlashSpiCommands;

  public:
    typedef struct MemAlloc{
      Address startAddress;
      uint32_t reservedSize;
    }MemAlloc;

  typedef enum S25fl512lError{
    S25fl512l_NO_ERROR = 0,
    S25fl512l_UNEXPECTED_ERROR = -1,
    S25fl512l_INCORRECT_FLASH_MEMORY = -2,
    S25fl512l_WRONG_DATA_SIZE = -3,
    S25fl512l_FAIL_ERASE_CHIP = -4,
    S25fl512l_FAIL_BLOCK_ERASE = -5,
    S25fl512l_FAIL_PAGE_PROGRAM = -6,
    S25fl512l_FAIL_SECTOR_ERASE = -7,
    S25fl512l_FAIL_MEM_ALLOCATION = -8,
    S25fl512l_FAIL_WRITE_DATA_FLASH = -9,
    S25fl512l_FAIL_HALF_BLOCK_ERASE = -10
  }S25fl512lError;

  //-------------------------------------------------------------------------------------------
  // Functions specific to external memory
  public:
    S25fl512l();
    ~S25fl512l() {}
    S25fl512lError writeDataToFlash(S25fl512l::MemAlloc *alloc,
                               const uint32_t offset,
                               uint8_t *data,
                               const uint16_t dataSize);
    S25fl512lError readDataFromFlash(S25fl512l::MemAlloc *alloc,
                                const uint32_t offset,
                                uint8_t *data,
                                const uint16_t dataSize);
    S25fl512lError setupDevice();
    S25fl512lError sectorErase(const S25fl512l::Sector sector);
  private:
    S25fl512lError pageProgram(S25fl512l::Address address,
                          uint8_t *txData,
                          const uint16_t size);

    S25fl512lError flashSpiReadData(const S25fl512l::FlashSpiCommands cmd,
                            uint8_t *rxData,
                            const uint16_t dataReadLength,
                            S25fl512l::Address address = ADDRESS_NOT_DEFINED);

    S25fl512lError flashSpiWriteData(const S25fl512l::FlashSpiCommands cmd,
                                uint8_t *txData = (uint8_t *)(NULL),
                                const uint16_t dataWriteLength = 0,
                                S25fl512l::Address address = ADDRESS_NOT_DEFINED);
    uint16_t getAddressLengthByte(const S25fl512l::FlashSpiCommands cmd);
    S25fl512lError allocateFlashMemory(S25fl512l::MemAlloc *alloc,
                                  const uint32_t size);

    S25fl512lError chipErase();
    S25fl512lError resetDevice();
    S25fl512lError programEraseResume();
    S25fl512lError programEraseSuspend();
    uint8_t getReadDummyCycles(const S25fl512l::FlashSpiCommands cmd);
    
  private:
    S25fl512l::AddressLengthFormat m_addressLengthFormat;

    //memory pointer that keeps track of the last allocated memory pointer
    S25fl512l::Address m_memAllocPointer;

    spiDAT1_t m_flashDataConfig;
    spiBASE_t *m_flashSpi;

    bool m_setup;
    uint32_t m_readLatencyCycles;
    uint16_t m_spiRxBuff[SPI_RX_BUFFER_MAX_LENGTH];
    uint16_t m_spiTxBuff[SPI_TX_BUFFER_MAX_LENGTH];
    uint8_t m_writeScratchpad[PAGE_SIZE];
    uint8_t m_dummyBuffer[MAX_DUMMY_CYCLES];
}; // end of class

#endif //__S25fl512l_HPP__
