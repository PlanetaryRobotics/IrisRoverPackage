#include "gio.h"
#include "spi.h"
#include "lin.h"
/*
S25fl512l g_fpgaFlash;
uint8_t g_cameraSelet = 0;      // 0 or 1

void fpgaTriggerCameraCapture(){
    uint16_t spiTxCmd = 0xFF;
    spiDAT1_t g_fpgaDataConfig;
    uint16_t rxWord = 0;

    g_fpgaDataConfig.CS_HOLD = false;
    g_fpgaDataConfig.DFSEL = SPI_FMT_0;
    g_fpgaDataConfig.WDEL = false;
    g_fpgaDataConfig.CSNR = 0;

    gioSetBit(spiPORT1, 0, 0); // set CS LOW

    // send data
    spiTransmitData(spiREG1, &g_fpgaDataConfig, 1, &spiTxCmd);

    gioSetBit(spiPORT1, 0, 1); // set CS HIGH
}

void getLineDummyImage(int line, uint8_t *dstBuff){
    memcpy(dstBuff, g_dummyImage + line*IMAGE_WIDTH, IMAGE_WIDTH);
}

void dummyFlashWrite(){
    uint8_t buffer[IMAGE_WIDTH];
    for(int i=0;i<sizeof(buffer);i++){
        buffer[i] = i%128;
    }
    S25fl512l::MemAlloc alloc;
    alloc.startAddress = 0;
    alloc.reservedSize = sizeof(buffer);

    for(int i=0; i<IMAGE_HEIGHT; i++){
        g_fpgaFlash.writeDataToFlash(&alloc, 0, buffer, sizeof(buffer));
        alloc.startAddress += PAGE_SIZE * 6;
    }
}

void eraseFpgaFlash(){
    for(int i=0; i< 40; i++){
        g_fpgaFlash.sectorErase(i);
    }
}

void downsampleLine(){
    for(uint32_t x=0; x<IMAGE_WIDTH/DOWNSAMPLING; x++){
        g_imageLineBuffer[x] = g_imageLineBuffer[x*DOWNSAMPLING];
    }
}

void getAndTransmitPicture(){
    imageTransferStatus_t status = BEGIN_NEW_TRANSFER;
    S25fl512l::MemAlloc alloc;
    alloc.startAddress = 0;
    alloc.reservedSize = 0;

    // set bit to control camera
    gioSetBit(linPORT, 1, g_cameraSelect & 0x01);

    eraseFpgaFlash();

    dummyFlashWrite();

    // add small delays to make sure camera is selection is done
    for(int delay=0; delay<500; delay++) asm("  NOP");

    fpgaTriggerCameraCapture();
    while(gioGetBit(gioPORTB, 1));

    for(int i=0;i<IMAGE_HEIGHT; i++){
#ifdef __USE_DUMMY_IMAGE__
        getLineDummyImage(i, g_imageLineBuffer);
#else
        g_fpgaFlash.readDataFromFlash(&alloc, 0, g_imageLineBuffer, sizeof(g_imageLineBuffer));
        alloc.startAddress = 6* PAGE_SIZE * i; // jump to next available block
#endif
        downsampleLine();
        status = (i == IMAGE_HEIGHT - 1) ? LAST_TRANSFER : CONTINUE;
        // sendCameraDataOverWifi(g_imageLineBuffer, sizeof(g_imageLineBuffer) / DOWNSAMPLING, status);
    }
}
*/
