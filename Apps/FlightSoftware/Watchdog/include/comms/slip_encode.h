#ifndef __WATCHDOG_SLIP_ENCODE_H__
#define __WATCHDOG_SLIP_ENCODE_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogSlipEncode SLIP Encoding
 * @addtogroup watchdogSlipEncode
 * @{
 */

/**
 * @brief Possible return statuses of SlipEncode functions.
 */
typedef enum SlipEncode__Status {
    SLIP_ENCODE__STATUS__OUTPUT_FULL = 1, //!< The next input byte, when encoded, cannot fit in the output buffer.
    SLIP_ENCODE__STATUS__INPUT_FINISHED = 0, //!< All input data has been encoded and put into the output buffer.
    SLIP_ENCODE__STATUS__ERROR_NULL = -1 //!< A required argument or a member of an argument was NULL.
} SlipEncode__Status;

/**
 * @brief SLIP encodes the contents of the input buffer and places the result in the output buffer until either the
 *        input is exhausted or the output is filled.
 *
 * @param input The input buffer. With each subsequent call of this function using the same input buffer, the value
 *              passed as this parameter should be offset by the cumulative `inputUsed` of all previous calls.
 * @param inputLen The remaining length of the data starting at `input`.
 * @param inputUsed A return parameter set to the number of bytes of the input that were consumed (i.e. successfully
 *                  encode and placed into the output buffer).
 * @param output The output buffer. With each subsequent call of this function using the same output buffer, the value
 *               passed as this parameter should be offset by the cumulative `outputUsed` of all previous calls.
 *               However, in the expected use of this function the output buffer will be fully used after each
 *               invocation of this function, in which case this parameter can always point to the beginning of the
 *               buffer.
 * @param outputLen The amount of space remaining in `output`.
 * @param outputUsed A return parameter set to the number of (previously) free bytes of the output that were utilized
 *                   in this function call. Even if the return status is `SLIP_ENCODE__STATUS__OUTPUT_FULL`, it is not
 *                   safe to assume that this value will be the same as `outputLen`, as some single input bytes become
 *                   two bytes after being encoded, meaning it's possible for the output to be unable to contain any
 *                   more encoded input data while still having one unused byte left.
 *
 * @return One of the following:
 *   - SLIP_ENCODE__STATUS__OUTPUT_FULL: The function was successful and filled the output before the input was
 *                                       exhausted.
 *   - SLIP_ENCODE__STATUS__INPUT_FINISHED: The function was successful and exhausted the input.
 *   - SLIP_ENCODE__STATUS__ERROR_NULL: `input`, `inputUsed`, `output`, or `outputUsed` was NULL.
 */
SlipEncode__Status SlipEncode__encode(const uint8_t* input,
                                      size_t inputLen,
                                      size_t* inputUsed,
                                      uint8_t* output,
                                      size_t outputLen,
                                      size_t* outputUsed);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif /* __WATCHDOG_SLIP_ENCODE_H__ */
