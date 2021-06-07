// ring_buffer.h

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stddef.h>

#include "include/common.h"

/**
 * @defgroup watchdogRingBuffer Ring Buffer
 * @addtogroup watchdogRingBuffer
 * @{
 */

/**
 * @brief An opaque structure that contains the internal state of a given RingBuffer instance.
 */
typedef struct RingBuffer RingBuffer;

/**
 * The maximum number of ring buffers that can be initialized.
 */
#define MAX_NUM_RING_BUFFERS 8

/**
 * @brief The possible return statuses of RingBuffer functions.
 */
typedef enum RingBuffer__Status {
    RB__STATUS__SUCCESS = 0, /** Operation succeeded. */
    RB__STATUS__ERROR__NULL = -1, /** A required argument or a member of an argument was NULL */
    RB__STATUS__ERROR__FULL = -2, /** Did not write byte because buffer is full */
    RB__STATUS__ERROR__EMPTY = -3, /** Could not read byte because buffer is empty */
    RB__STATUS__ERROR__NOT_POWER_OF_TWO = -4, /** Buffer size was not a power of two */
    RB__STATUS__ERROR__ZERO_SIZE = -5, /** Buffer size was zero */
    RB__STATUS__ERROR__ALL_BUFFERS_USED = -6, /** All statically allocated RingBuffers already in use */

    RB__STATUS__ERROR__INTERNAL = -255 /** An unexpected internal error occurred. */
} RingBuffer__Status;

/**
 * @brief Initializes a given RingBuffer instance.
 *
 * @param rb An output parameter into which the address of the initialized RingBuffer will be placed if this function
 *           succeeds.
 * @param buffer The data buffer to use as the backing memory of the ring buffer.
 * @param bufferSize The size of `buffer`. MUST be a power of two.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb` or `buffer` was NULL.
 *   - RB__STATUS__ERROR__NOT_POWER_OF_TWO: `bufferSize` was not a power of two.
 *   - RB__STATUS__ERROR__ZERO_SIZE: `bufferSize` was zero.
 *   - RB__STATUS__ERROR__ALL_BUFFERS_USED: All statically allocated RingBuffers have already been used.
 */
RingBuffer__Status RingBuffer__init(RingBuffer** rb, volatile uint8_t* buffer, size_t bufferSize);

// These should only be called with interrupts disabled if the ring buffer in question is 
// touched at all from any ISR
/**
 * @brief Checks if the ring buffer is full.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 *
 * @return TRUE if full, otherwise FALSE. Also FALSE if `rb` is NULL.
 */
BOOL RingBuffer__full(const RingBuffer* rb);

/**
 * @brief Checks if the ring buffer is empty.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 *
 * @return TRUE if empty, otherwise FALSE. Also FALSE if `rb` is NULL.
 */
BOOL RingBuffer__empty(const RingBuffer* rb);

/**
 * @brief Gets the count of unused slots in the ring buffer.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 *
 * @return The number of unused slots in the ring buffer. Will be zero if `rb` is NULL.
 */
size_t RingBuffer__freeCount(const RingBuffer* rb);

/**
 * @brief Gets the count of occupied slots in the ring buffer.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 *
 * @return The number of occupied slots in the ring buffer. Will be zero if `rb` is NULL.
 */
size_t RingBuffer__usedCount(const RingBuffer* rb);

/**
 * @brief Gets the value from a particular index in the ring buffer without modifying the ring buffer in any way.
 *
 * The index is indexed relative to the current head of the ring buffer, and will be wrapped around if needed.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 * @param index The index to access.
 * @param value An output parameter for the value retrieved from the specific index, only set if this function returns
 *              `RB__STATUS__SUCCESS`.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb` or `rb->buffer` was NULL.
 *   - RB__STATUS__ERROR__EMPTY: The ring buffer is empty at `index`.
 */
RingBuffer__Status RingBuffer__peekAt(const RingBuffer* rb, size_t index, uint8_t* value);

/**
 * @brief Puts the given byte at the end of the ring buffer if there is space. If the ring buffer is full, the given
 *        byte will not be put onto the ring buffer.
 *
 * This, along with `RingBuffer__get()`, is safe to call in any context (in ISR or not) without needing to disable any
 * interrupts. This is true because of the following:
 *
 * If put() is called in an interrupt and get() called outside of an interrupt, then these two functions can be used
 * safely without disabling any interrupts. put() calls full(), which reads both the head and tail, and then may modify
 * head. get() calls empty(), which reads both the head and tail, and then may modify tail. In this scenario calling
 * put() is safe because it is called within the interrupt, so get() can't modify the head or tail while it is executed.
 * Additionally, calling get() is safe because even though it *is* possible for head to be modified, this modification
 * will only cause get() to not catch that a new element to be added to the buffer, but the next time get() is called
 * it will be noticed.
 *
 * If get() is called in interrupt and put() is called outside an interrupt, then these functions are still safe to use.
 * The worst thing that can happen is that during a a call to put(), the get() is invoked from the interrupt. In this
 * case, the get() call may free a slot in the buffer, but the call to put() won't see this free slot and the put() call
 * will fail to write the byte to the buffer. Similarly, the get() call may not see a byte in the process of being
 * written if the put() call is interrupted, but it is expected that after calling put() the code should make sure that
 * whatever code calls get() is invoked (i.e. by enabling the appropriate interrupt).
 *
 * @param rb The ring buffer instance.
 * @param byte The value to put into the ring buffer.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb` or `rb->buffer` was NULL.
 *   - RB__STATUS__ERROR__FULL: The ring buffer was full.
 */
RingBuffer__Status RingBuffer__put(RingBuffer* rb, uint8_t byte);

/**
 * @brief Pops and returns the oldest byte in the ring buffer.
 *
 * This, along with `RingBuffer__put()`, is safe to call in any context (in ISR or not) without needing to disable any
 * interrupts. See the documentation of `RingBuffer__put()` for the justification of this claim.
 *
 * @param rb The ring buffer instance.
 * @param byte An output parameter for the byte retrieved from the ring buffer, only set if the return status is
 *             `RB__STATUS__SUCCESS`.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb`, `rb->buffer`, or `byte` was NULL.
 *   - RB__STATUS__ERROR__EMPTY: The ring buffer was empty.
 */
RingBuffer__Status RingBuffer__get(RingBuffer* rb, uint8_t* byte);

/**
 * @brief Puts the given byte at the end of the ring buffer, overwriting the oldest byte in the ring buffer if the
 *        ring buffer was full.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR. This is because
 *          putOverwrite() may modify both head *and* tail, so not disabling the interrupt will cause a race condition.
 *
 * @param rb The ring buffer instance.
 * @param byte The value to put into the ring buffer.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb` or `rb->buffer` was NULL.
 */
RingBuffer__Status RingBuffer__putOverwrite(RingBuffer* rb, uint8_t byte);

/**
 * @brief Pops and returns the oldest byte from the ring buffer.
 *
 * @note This function is actually the same as `RingBuffer__get()`, but is implemented as a separate function to make
 *       it clear that interrupts need to be disabled when making this call in (see warning in the documentation of
 *       `RingBuffer__putOverwrite()` for the reasoning why).
 *
 * @param rb The ring buffer instance.
 * @param byte An output parameter for the byte retrieved from the ring buffer, only set if the return status is
 *             `RB__STATUS__SUCCESS`.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb`, `rb->buffer`, or `byte` was NULL.
 *   - RB__STATUS__ERROR__EMPTY: The ring buffer was empty.
 */
RingBuffer__Status RingBuffer__getOverwrite(RingBuffer* rb, uint8_t* byte);

/**
 * @brief Clears the ring buffer.
 *
 * @warning This should only be called with interrupts disabled if `rb` is touched at all from any ISR.
 *
 * @param rb The ring buffer instance.
 *
 * @return One of the following:
 *   - RB__STATUS__SUCCESS: The function was successful.
 *   - RB__STATUS__ERROR__NULL: `rb` was NULL.
 */
RingBuffer__Status RingBuffer__clear(RingBuffer* rb);

/**
 * @}
 */

#endif // #ifndef __RING_BUFFER_H__
