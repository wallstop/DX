/* /////////////////////////////////////////////////////////////////////////////////////////////////
    DX LockFree - A high-level RAII concurrency library designed for fast and easy use
    Copyright(C) 2014 Eli Pinkerton

    This library is free software; you can redistribute it and / or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or(at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301  USA
*/ /////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// DX LockFree - Abstract Barrier to allow for multithreaded synchronized waits
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../CacheLine.h"

#include <atomic>

namespace DX {
namespace LockFree {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // AbstractBarrier


    /*! \brief An AbstractBarrier is designed for use between multiple threads, typically blocking
        unparallelelizable sections of code. Any class derived off of an Abstract Barrier will block
        threads of execution at calls to wait() until wait() has been called some number of times,
        where that amount is defined by the number supplied to the constructor.

        Behavior is undefined if the number of threads to wait on is 0 or 1.
        
        A contrived use case is for scatter and gather:

        \code

        void processWork(AbstractBarrier& barrier)
        {
            // Do some work
            barrier.wait();
        }

        void scatter()
        {
            // + 1 here, because we want to block the main thread of execution as well
            AbstractBarrier barrier(someNumberOfThreads );
            for(int i = 0; i < someNumberOfThreads; ++i)
                std::thread(&processWork, barrier);

            barrier.wait();
            gather();
        }

        void gather()
        {
            // Do some other stuff 

            //Here, scatter() has ensured that gather will ONLY be called once all threads have
            //completed their work.
        }
        \endcode
    */
    class AbstractBarrier
    {
    public:
        /*! Constructs an AbstractBarrier that is set up to wait on some number of threads
            \param[in] numThreads   The number of threads that are expected to use this barrier
        */
        explicit AbstractBarrier(size_t numThreads);
        virtual ~AbstractBarrier();

        /*! Blocks the current thread of execution until all other wait() calls have beem made */
        virtual void wait() const = 0;

    protected:
        volatile char pad_0[CACHE_LINE_SIZE];
        mutable std::atomic<size_t> m_count;
        volatile char pad_1[CACHE_LINE_SIZE - (sizeof(std::atomic<size_t>) % CACHE_LINE_SIZE)];

    private:
        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        AbstractBarrier(const AbstractBarrier&);
        AbstractBarrier(AbstractBarrier&&);
    };

}
}