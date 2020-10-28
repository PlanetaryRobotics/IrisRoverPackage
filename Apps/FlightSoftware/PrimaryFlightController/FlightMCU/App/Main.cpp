/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include <CubeRoverConfig.hpp>
#include "FreeRTOS.h"
#include "os_task.h"
#include "CubeRover/Top/Topology.hpp"
#include "CubeRover/NetworkManager/CubeRoverNetworkManager.hpp"

#include "gio.h"
#include "i2c.h"

extern "C" {
    void vApplicationIdleHook(void);
}

void vApplicationIdleHook(void) {
    run1cycle();
    //gioToggleBit(gioPORTB, 1);
    //for(uint32_t i=0; i<2000000; i++) asm("  NOP");
}

using namespace Wf121;

uint8_t g_testBuffer[RX_RING_BUFFER_SIZE];

void main(void)
{
    CubeRoverNetworkManager wf121;
    uint16_t headerSize = 8;    // 8 bytes
    uint16_t byteRead = 0;
    uint32_t payloadSize = 0;

    /* USER CODE BEGIN (3) */
    gioInit();

    i2cInit();
    sciInit();

    constructApp();

    while(1){
        wf121.UpdateNetworkManager();

        wf121.ReceiveUdpData(g_testBuffer, headerSize, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::PEEK_READ, 10);

        if(byteRead == headerSize){
            // get size of the received packet
            // check how big the packet actually is, then consume the bytes from the ring buffer
            memcpy(&payloadSize, g_testBuffer+4 /* offset by packet number */, sizeof(payloadSize));
            // FIXME: payloadsize is U32 but in UDP header length is only bytes 4 and 5 (U16)
            wf121.ReceiveUdpData(g_testBuffer, payloadSize, &byteRead, UdpReadMode::WAIT_UNTIL_READY | UdpReadMode::NORMAL_READ, 10);

            // send back signal quality
            g_testBuffer[8] = wf121.GetSignalRssi();
            g_testBuffer[9] = wf121.GetSignalNoiseRatio();
            sciSend(scilinREG, payloadSize, g_testBuffer);
            wf121.SendUdpData(g_testBuffer, payloadSize, 10000);
        }
    }

    vTaskStartScheduler();

    //if it reaches that point, there is a problem with RTOS.

/* USER CODE END */
}
