/*
 * Main.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: cedric
 */

#include "sys_common.h"
#include "sys_core.h"
#include "system.h"

#include "FreeRTOS.h"
#include "os_task.h"

#include "adc.h"
#include "gio.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "rti.h"
#include "sys_dma.h"
#include "sys_mpu.h"

#include "App/DMA.h"
#include "App/SCI.h"
#include "App/SCILIN.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cassert>


static volatile bool dmaWriteBusy = false;

extern "C" void SCILIN_TX_DMA_ISR(dmaInterrupt_t inttype)
{
    dmaWriteBusy = false;
}

static const TickType_t DMA_SEND_POLLING_CHECK_INTERVAL = 2 / portTICK_PERIOD_MS; // every 2ms (2 ticks)

#define POINTER_CAST U32
typedef uint32_t       U32; //!< 32-bit signed integer

class Mutex {
    public:

        Mutex(void); //!<  Constructor. Mutex is unlocked when created
        virtual ~Mutex(void); //!<  Destructor

        void lock(void); //!<  lock the mutex
        void unLock(void); //!<  unlock the mutex

    private:

        POINTER_CAST m_handle; //!<  Stored handle to mutex
};

// The sci base used to initialize the watchdog interface connection
sciBASE_t *m_sci;

char m_printBuffer[256];
bool m_finished_initializing;                      // Flag set when this component is fully initialized and interrupt DMA can be used (otherwise polling DMA)

typedef int NATIVE_INT_TYPE; //!< native integer type declaration
NATIVE_INT_TYPE strnlen(const char *s, NATIVE_INT_TYPE maxlen);

void init_uart_test()
{
    m_sci = scilinREG;
}

// FIXME: Add timeout to escape polling loop
void pollDMASendFinished()
{
    if (!m_finished_initializing)
    {
        while (!((getDMAIntStatus(BTC) >> SCILIN_TX_DMA_CH) & 0x01U))
        {
            vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
        };
        dmaWriteBusy = false;
        sciDMASendCleanup(SCILIN_TX_DMA_CH);
    }

    while (dmaWriteBusy)
    {
        vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
    };
    ; // TODO: Mutex to allow multiprogramming & TIMEOOUT
}

// Returns negative on error
bool dmaSend(void *buffer, int size, bool blocking)
{
    static Mutex resourceProtectionMutex; // quick and dirty. prevents multiple Tasks from trying this at once
    // sciSend(m_sci, size, static_cast<uint8_t *>(buffer));
    // return true;
    resourceProtectionMutex.lock();

    if (blocking)
    {
        while (dmaWriteBusy)
        {
            vTaskDelay(DMA_SEND_POLLING_CHECK_INTERVAL); // Check back in every X ticks (don't hog the processor)
        };
    }
    else if (dmaWriteBusy)
    {
        return false;
    }
    sciDMASend(SCILIN_TX_DMA_CH, static_cast<char *>(buffer), size, ACCESS_8_BIT, &dmaWriteBusy);
    if (blocking)
        pollDMASendFinished();

    resourceProtectionMutex.unLock();
    return true;
}

bool debugPrintfToWatchdog(const char *fmt, ...)
{
    static Mutex sloppyResourceProtectionMutex; // quick and dirty. keeps multiple tasks from doing this at once.
    if (fmt == NULL)
    {
        return false;
    }

    sloppyResourceProtectionMutex.lock();
    memset(m_printBuffer, 0, sizeof(m_printBuffer));
    sprintf(m_printBuffer, "DEBUG");
    va_list args;
    va_start(args, fmt); // @suppress("Function cannot be resolved")
    vsnprintf(m_printBuffer + 5, sizeof(m_printBuffer) - 5, fmt, args);
    va_end(args);

    uint8_t *dataBuffer = reinterpret_cast<uint8_t *>(m_printBuffer);
    size_t dataLen = static_cast<size_t>(strnlen(m_printBuffer, sizeof(m_printBuffer)));

    bool success = dmaSend(dataBuffer, dataLen, true);;

    sloppyResourceProtectionMutex.unLock();

    return success;
}

void main(void)
{
    /* USER CODE BEGIN (3) */
    _disable_interrupt_(); // Disable all interrupts during initialization (esp. important when we initialize RTI)
    _mpuInit_();

    gioInit();
    sciInit();
    dmaEnable();
    scidmaInit();
    sciIntInit();
    scilinIntInit();

    sciEnterResetState(m_sci);
    sciSetBaudrate(m_sci, 57600);
    sciExitResetState(m_sci);

    init_uart_test();

    /* Block for 1s (1000ms) after each uart print attempt. */
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
    uint16_t uart_print_counter = 1;
    uint16_t uart_print_success = 1;

    while(1) {
        if (debugPrintfToWatchdog("Hercules UART %u of %u\n", uart_print_success, uart_print_counter)) {
            uart_print_success = uart_print_success + 1;
        }

        uart_print_counter = uart_print_counter + 1;
        vTaskDelay( xDelay );
    }

}

// -RAD-


