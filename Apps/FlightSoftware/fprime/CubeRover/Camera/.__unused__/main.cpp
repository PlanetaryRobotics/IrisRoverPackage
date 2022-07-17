#include "FreeRTOS.h"
#include "os_task.h"
#include "sys_core.h"

#include "main.hpp"
#include "Wf121.hpp"
#include "epos.hpp"
#include "NetworkManager/CubeRoverNetworkManager.hpp"
#include "S25fl512l.hpp"

#include "gio.h"
#include "spi.h"
#include "adc.h"
#include "lin.h"

extern "C" {
    void vApplicationIdleHook( void );
}

void vApplicationIdleHook( void ){

}

uint8_t g_imageLineBuffer[IMAGE_WIDTH];
//uint16_t g_spiRxBuffer[IMAGE_WIDTH];
S25fl512l g_fpgaFlash;
uint8_t g_cameraSelect;

CubeRoverNetworkManager g_wf121;

#define __USE_DUMMY_IMAGE__ 1

#ifdef __USE_DUMMY_IMAGE__
uint8_t g_dummyImage[IMAGE_WIDTH * IMAGE_HEIGHT];
void generateDummyImage(){
   for(int x=0; x< IMAGE_WIDTH; x++){
       for(int y=0; y< IMAGE_HEIGHT; y++){
           g_dummyImage[x+y*IMAGE_WIDTH] = x%255 + y%255;
       }
   }
}
#endif

bool g_takePictureFlag;

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

void sendCameraDataOverWifi(uint8_t *dataBuffer, uint32_t dataBuffSize, const imageTransferStatus_t status){
    uint32_t payloadBytesToSend = dataBuffSize;
    uint32_t offset = 0;
    imageTransferStatus_t st = status;

    while(payloadBytesToSend){
        g_txBuffer[0] = TELEM_CAMERA_HEADER;

        int dataPayloadSize = (payloadBytesToSend < TX_RX_UDP_BUFF_SIZE-6) ? payloadBytesToSend : TX_RX_UDP_BUFF_SIZE-6;

        g_txBuffer[1] =  CONTINUE;

        if(st == BEGIN_NEW_TRANSFER){
            g_txBuffer[1] = BEGIN_NEW_TRANSFER;
        }

        if(st == LAST_TRANSFER){
            g_txBuffer[1] = (dataPayloadSize < sizeof(g_txBuffer)-6) ? LAST_TRANSFER : CONTINUE;
        }

        g_txBuffer[2] = dataPayloadSize;
        g_txBuffer[3] = dataPayloadSize >> 8;
        g_txBuffer[4] = dataPayloadSize >> 16;
        g_txBuffer[5] = dataPayloadSize >> 24;

        memcpy(g_txBuffer+6, dataBuffer + offset, dataPayloadSize);
        g_wf121.SendUdpData(g_txBuffer, dataPayloadSize+6, 100000);
        payloadBytesToSend -= dataPayloadSize;
        offset += dataPayloadSize;
    }
}

#ifdef __USE_DUMMY_IMAGE__
void getLineDummyImage(int line, uint8_t *dstBuff){
    memcpy(dstBuff, g_dummyImage + line*IMAGE_WIDTH, IMAGE_WIDTH);
}
#endif

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

void downsampleLine(){
    for(uint32_t x=0; x<IMAGE_WIDTH/DOWNSAMPLING; x++){
        g_imageLineBuffer[x] = g_imageLineBuffer[x*DOWNSAMPLING];
    }
}

void eraseFpgaFlash(){
    for(int i=0; i< 40; i++){
        g_fpgaFlash.sectorErase(i);
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
        sendCameraDataOverWifi(g_imageLineBuffer, sizeof(g_imageLineBuffer) / DOWNSAMPLING, status);
    }
}

int processCameraCmd(uint8_t *camData){
    switch(camData[0]){
        case TAKE_PICTURE_CAM_0:
            g_cameraSelect = 0;
            break;
        case TAKE_PICTURE_CAM_1:
            g_cameraSelect = 1;
            break;
        case GET_PICTURE:
            g_takePictureFlag = true;
            break;
    }

    return CMD_NO_ERROR;
}

int main(void){
    uint16_t byteRead = 0;
    int32_t pos[4] = {0};
    int32_t current[4] = {0};
    int32_t velocity[4] = {0};

    uint32_t preScalerTask = 0;

    g_cameraSelect = 0;
    g_takePictureFlag = false;

    preScalerTemperature = 0;

    /* USER CODE BEGIN (3) */
    spiInit();

    g_fpgaFlash.setupDevice();

    for(uint32_t delay = 100000; delay >0; delay--);
    _enable_interrupt_();

#ifdef __USE_DUMMY_IMAGE__
    generateDummyImage();
#endif

    // main loop
    while(1){
        
       if(g_takePictureFlag){
           getAndTransmitPicture();
           g_takePictureFlag = false;
       }

        if(preScalerTask++ % 1000){
                g_wf121.ReceiveUdpData(g_rxBuffer, 8, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);
        }
    }
}
