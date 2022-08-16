/**
 * @file SimpleFifoBuffer.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Simple Array-based Fixed-size FIFO Queue Buffer implementation used
 * for Loading data from the WF121 Radio, with the ability for (cautious)
 * asynchronous enqueueing.
 *
 *
 * NOTE: The `tail_ptr` and `advance_tail` functions allow for asynchronous
 * enqueuing. An example of this is we give `tail_ptr` to sciReceive to load a
 * byte into, and once we receive the ISR saying that it read a byte, we
 * immediately advance the tail.
 * Use cautiously since this could make it possible to write a value
 * and then read from it before the tail is advanced, leading to lost
 * data. For this particular designed use-case mentioned above, this is simple
 * and sufficient.
 * So, if you use this feature, make sure:
    1. Only ever add one value at a time.
    2. You *always* call `advance_tail` after the data is added and before it's
    ever read.
 *
 * @date 2022-08-11
 * @copyright Copyright (c) 2022
 */

#ifndef CUBEROVER_WF121_SIMPLE_FIFO_BUFFER_HPP_
#define CUBEROVER_WF121_SIMPLE_FIFO_BUFFER_HPP_

namespace Wf121
{
    template <typename T, unsigned int N, typename ST = unsigned int>
    struct SimpleAsyncFifoBuffer
    {
        T values[N];
        ST head;
        ST tail;
        ST num_entries;
        const ST size;

        SimpleAsyncFifoBuffer() : size(N)
        {
            reset();
        }

        // Reset / initialize the metadata:
        void reset()
        {
            num_entries = 0;
            head = 0;
            tail = 0;
        }

        inline bool is_empty() { return num_entries == 0; }

        inline bool is_full() { return num_entries == N; }

        // Enqueues, returning success.
        bool enqueue(T value)
        {
            if (is_full())
            {
                return false;
            }
            values[tail] = value;
            tail = (tail + 1) % N;
            num_entries++;
            return true;
        }

        // Dequeues a value, simply "removing it" (advancing the head past it).
        // Returns if anything was actually "removed".
        bool dequeue()
        {
            if (is_empty())
            {
                return false;
            }
            // Just advance the head:
            head = (head + 1) % N;
            num_entries--;
            return true;
        }

        // Dequeues a value, putting it into `valPtr`. Returning success.
        bool dequeue_into(T *valPtr)
        {
            if (is_empty())
            {
                return false;
            }

            *valPtr = values[head];
            head = (head + 1) % N;
            num_entries--;
            return true;
        }

        T *tail_ptr()
        {
            return &values + tail;
        }

        bool advance_tail()
        {
            tail = (tail + 1) % N;
            num_entries += 1;

            // Make sure num_entries didn't exceed size (something went wrong):
            if (num_entries > N)
            {
                num_entries = N;
                return false;
            }
            else
            {
                return true;
            }
        }

        // Reads out every available value into the given target array without
        // dequeuing, but in the order they would be dequeued in.
        // Make sure target has at least N values available.
        // Returns how many values were read.
        ST straighten_into(T *target)
        {
            // Copy all the data, in order, into given buffer:
            ST read_head = head;

            for (ST i = 0; i < num_entries; i++)
            {
                target[i] = values[read_head];
                read_head = (read_head + 1) % N;
            }

            return num_entries;
        }
    };

}

#endif /* CUBEROVER_WF121_SIMPLE_FIFO_BUFFER_HPP_ */