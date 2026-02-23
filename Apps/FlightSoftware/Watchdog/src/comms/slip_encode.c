#include "comms/slip_encode.h"
#include "common.h"

SlipEncode__Status SlipEncode__encode(const uint8_t* input,
                                      size_t inputLen,
                                      size_t* inputUsed,
                                      uint8_t* output,
                                      size_t outputLen,
                                      size_t* outputUsed)
{
    if (NULL == input || NULL == inputUsed || NULL == output || NULL == outputUsed) {
        return SLIP_ENCODE__STATUS__ERROR_NULL;
    }

    *inputUsed = 0;
    *outputUsed = 0;

    size_t outIndex = 0;
    size_t inIndex;

    for (inIndex = 0; inIndex < inputLen; ++inIndex) {
        uint8_t inByte = input[inIndex];
        size_t lastByteIndexOffset = (inByte == SLIP_END || inByte == SLIP_ESC) ? 1 : 0;

        if (outIndex + lastByteIndexOffset < outputLen) {
            // There is room for the SLIP-encoded output for this input byte
            if (inByte == SLIP_END) {
                output[outIndex] = SLIP_ESC;
                output[outIndex + 1] = SLIP_ESC_END;
                outIndex += 2;
            } else if (inByte == SLIP_ESC) {
                output[outIndex] = SLIP_ESC;
                output[outIndex + 1] = SLIP_ESC_ESC;
                outIndex += 2;
            } else {
                output[outIndex] = inByte;
                outIndex++;
            }
        } else {
            *inputUsed = inIndex;
            *outputUsed = outIndex;
             return SLIP_ENCODE__STATUS__OUTPUT_FULL;
        }
    }

    *inputUsed = inIndex;
    *outputUsed = outIndex;
    return SLIP_ENCODE__STATUS__INPUT_FINISHED;
}
