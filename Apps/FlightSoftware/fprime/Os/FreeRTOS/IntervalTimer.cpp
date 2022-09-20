#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <HAL/include/FreeRTOS.h>
#include <HAL/include/os_task.h>
#include <string.h>

namespace Os
{

    /**
     * @brief      Constructs the object.
     */
    IntervalTimer::IntervalTimer()
    {
        memset(&this->m_startTime, 0, sizeof(this->m_startTime));
        memset(&this->m_stopTime, 0, sizeof(this->m_stopTime));
    }

    /**
     * @brief      Destroys the object.
     */
    IntervalTimer::~IntervalTimer()
    {
    }

    /**
     * @brief      Gets the timer frequency.
     *
     * @return     The timer frequency.
     */
    U32 IntervalTimer::getTimerFrequency(void)
    {
        return 1;
    }

    /**
     * @brief      Gets the raw time.
     *
     * @param      time  The time
     */
    void IntervalTimer::getRawTime(RawTime &time)
    {

        TickType_t xTime1;

        /* Get the time the function started. */
        xTime1 = xTaskGetTickCount();
        // Fixed timer (previously it was passing ticks **into** a
        // ticks-to-milliseconds converter (treated as a ms-to-ticks converter)
        // then multiplying by 1000 (to convert to seconds)?)
        // Interestingly, this didn't actually matter since tick rate was 1000Hz
        // (so *1000 /1000) but if that were changed, it would.
        // Now it just converts ticks to seconds.
        time.lower = xTime1 / configTICK_RATE_HZ;
        time.upper = 0;
    }

    /**
     * @brief      Gets the difference usec.
     *
     * @return     The difference usec.
     */
    U32 IntervalTimer::getDiffUsec(void)
    {
        return getDiffUsec(this->m_stopTime, this->m_startTime);
    }

    // should be t1In - t2In

    /**
     * @brief      Gets the difference usec.
     *
     * @param[in]  t1In  The t 1 in
     * @param[in]  t2In  The t 2 in
     *
     * @return     The difference usec.
     * @note       Adapted from:
     *             http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
     */
    U32 IntervalTimer::getDiffUsec(const RawTime &t1In, const RawTime &t2In)
    {

        RawTime result = {t1In.upper - t2In.upper, 0};

        if (t1In.lower < t2In.lower)
        {
            result.upper -= 1; // subtract nsec carry to seconds
            result.lower = t1In.lower + (1000000000 - t2In.lower);
        }
        else
        {
            result.lower = t1In.lower - t2In.lower;
        }

        return result.lower;
    }

    /**
     * @brief      Start the timer
     */
    void IntervalTimer::start()
    {
        getRawTime(this->m_startTime);
    }

    /**
     * @brief      Stop the timer
     */
    void IntervalTimer::stop()
    {
        getRawTime(this->m_stopTime);
    }

    /**
     * @brief      Gets the difference raw.
     *
     * @param[in]  t1    The t 1
     * @param[in]  t2    The t 2
     *
     * @return     The difference raw.
     */
    IntervalTimer::RawTime IntervalTimer::getDiffRaw(const RawTime &t1, const RawTime &t2)
    {
        RawTime diff;
        const U64 al = (((U64)t1.upper << 32) + (U64)t1.lower);
        const U64 bl = (((U64)t2.upper << 32) + (U64)t2.lower);
        if (t2.lower > t1.lower)
        {
            diff.lower = 0xFFFFFFFF - (t2.lower - t1.lower - 1);
            diff.upper = t1.upper - t2.upper - 1;
        }
        else
        {
            diff.lower = (U32)(al - bl);
            diff.upper = (U32)((U64)(al - bl) >> 32);
        }
        return diff;
    }

}
