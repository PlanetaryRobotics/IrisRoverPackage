#ifndef __WATCHDOG_SLIP_ENCODE_H__
#define __WATCHDOG_SLIP_ENCODE_H__

#include <stdint.h>
#include <stddef.h>

typedef enum SlipEncode__Status {
    SLIP_ENCODE__STATUS__OUTPUT_FULL = 1, /*!< The next input byte, when encoded, cannot fit in the output buffer */
    SLIP_ENCODE__STATUS__INPUT_FINISHED = 0 /*!< All input data has been encoded and put into the output buffer */
} SlipEncode__Status;

SlipEncode__Status SlipEncode__encode(const uint8_t* input,
                                      size_t inputLen,
                                      size_t* inputUsed,
                                      uint8_t* output,
                                      size_t outputLen,
                                      size_t* outputUsed);

#endif /* __WATCHDOG_SLIP_ENCODE_H__ */
