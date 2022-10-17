#ifndef SPI_H_
#define SPI_H_

#include <hal_stdtypes.h>
#include <spi.h>


#define RW_OFFSET       7
#define READ_BIT        1
#define WRITE_BIT       0
#define MB_OFFSET       6


#ifdef __cplusplus
extern "C" {
#endif
    

uint32 spiTransmitOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16* srcbuff);
uint32 spiReceiveOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16 *destbuff);
void read_multibyte(spiBASE_t *spi, spiDAT1_t *dataconfig, uint8 startAddr, uint8 count, uint8 *buf);
void write_register(spiBASE_t *spi, spiDAT1_t *dataconfig, uint8 addr, uint8 value);
uint8 read_register(spiBASE_t *spi, spiDAT1_t *dataconfig, uint8 addr);


#ifdef __cplusplus
}
#endif

#endif  // SPI_H_
