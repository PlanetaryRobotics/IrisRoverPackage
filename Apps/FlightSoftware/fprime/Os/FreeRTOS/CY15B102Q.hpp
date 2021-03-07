#ifndef __CY15B102Q_HPP__
#define __CY15B102Q_HPP__

#include <spi.h>
#include <string.h>
#include <gio.h>
#include <math.h>

#define SPI_RX_BUFFER_MAX_LENGTH_FRAM 261    // Worst case: 256 data payload bytes + 1 byte command + 4 bytes addressing
#define SPI_TX_BUFFER_MAX_LENGTH_FRAM 261

#define SPI_REG_FRAM spiREG4
#define CS_SPI_PORT_FRAM spiPORT4
#define CS_SPI_BIT_FRAM  0

// Address lengths are 3 bytes (checked)
#define AddressLengthByte 3;

#define DEFAULT_DUMMY_CYCLES 8
#define MAX_DUMMY_CYCLES     16

#define MAX_MEMORY_ADDRESS_FRAM       0x3FFFF            // .25Mb

#define ADDRESS_NOT_DEFINED     0xFFFFF

#define DEVICE_ID_1   0x7F7F7F7F7F7FC225
#define DEVICE_ID_2   0xC8

class CY15B102Q{
  
  public:

  typedef uint32_t Address;
  uint32_t m_readLatencyCycles;

  enum AddressLengthFormat{
    ADDRESS_LENGTH_3_BYTES  = 3,
    ADDRESS_LENGTH_4_BYTES  = 4
  };

  struct DEVICEIDBITS{
    uint8_t rsvd:3;
    uint8_t rev:3;
    uint8_t sub:3;
    uint8_t density:3;
    uint8_t family:3;
    uint64_t manufacturerid:56;
  };

  typedef union DEVICEID{
    uint8_t all;
    DEVICEIDBITS bit;
  }DEVICEID;

  struct StatusRegisterBits{
    uint8_t bit0:1;
    uint8_t wel:1;
    uint8_t bp0:1;
    uint8_t bp1:1;
    uint8_t bit4:1;
    uint8_t bit5:1;
    uint8_t bit6:1;
    uint8_t wpen:1;
  };

  typedef union StatusRegister{
    uint8_t all;
    StatusRegisterBits bit;
  }StatusRegister;

  typedef enum FRAMSpiCommands{
    WREN = 0x06,    // Set Write Enable Latch
    WRDI = 0x04,    // Reset Write Enable Latch
    RDSR = 0x05,    // Read Status Register
    WRSR = 0x01,    // Write Status Register
    READ = 0x03,    // Read Memory Data
    FSRTD = 0x0B,   // Fast Read Memory Data
    WRITE = 0x02,   // Write Memory Data
    SLEEP = 0xB9,   // Enter Sleep Mode
    RDID = 0x9F     // Read Device ID
  }FRAMSpiCommands;

    typedef struct MemAlloc{
      Address startAddress;
      uint32_t reservedSize;
    }MemAlloc;

    typedef enum CY15B102QError{
    CY15B102Q_NO_ERROR = 0,
    CY15B102Q_UNEXPECTED_ERROR = -1,
    CY15B102Q_INCORRECT_FLASH_MEMORY = -2,
    CY15B102Q_WRONG_DATA_SIZE = -3,
    CY15B102Q_FAIL_ERASE_CHIP = -4,
    }CY15B102QError;

  //-------------------------------------------------------------------------------------------
  // Functions specific to external memory
  public:

    CY15B102Q();
    //~CY15B102Q();

    uint8_t getReadDummyCycles(const CY15B102Q::FRAMSpiCommands cmd);

    CY15B102QError setupDevice();

    CY15B102QError framSpiReadData(const CY15B102Q::FRAMSpiCommands cmd,
                            uint8_t *rxData,
                            const uint16_t dataReadLength,
                            uint32_t address); 

    CY15B102QError framSpiWriteData(const CY15B102Q::FRAMSpiCommands cmd,
                                uint8_t *txData,
                                const uint16_t dataWriteLength,
                                uint32_t address); 
    
  private:
    spiDAT1_t m_framDataConfig;
    spiBASE_t *m_framSpi;

    //bool m_setup;
    uint16_t m_spiRxBuff[SPI_RX_BUFFER_MAX_LENGTH_FRAM];
    uint16_t m_spiTxBuff[SPI_TX_BUFFER_MAX_LENGTH_FRAM];
}; // end of class

#endif //__CY15B102Q_HPP__
