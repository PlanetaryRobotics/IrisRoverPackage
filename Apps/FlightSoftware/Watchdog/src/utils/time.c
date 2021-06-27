#include "include/utils/time.h"

static volatile uint16_t centisecondCount;

void Time__resetToZero(void)
{
    centisecondCount = 0;
}

volatile uint16_t* Time__getPointerToCentisecondCount(void)
{
    return &centisecondCount;
}

uint16_t Time__getTimeInCentiseconds(void)
{
    return centisecondCount;
}
