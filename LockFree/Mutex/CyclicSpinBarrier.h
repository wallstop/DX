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
// DX LockFree - CyclicBarrier is a self-resetting AbstractBarrier implementation
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AbstractBarrier.h"
#include "SpinRWMutex.h"

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // CyclicSpinBarrier

    /*! \brief CyclicSpinBarrier is an AbstractBarrier implementation that will reset itself after all
        calls to wait() have unblocked. This allows for barrier use within loops that require blocking
        at the end, or other similar scenarios.

        \code
        void process(CyclicSpinBarrier& barrier1, CyclicSpinBarrier& barrier2)
        {
            while(true)
            {
                // Do some work
                barrier1.wait();
                barrier2.wait();
            }
        }

        int main(int argc, char* argv[])
        {
            CyclicSpinBarrier innerBarrier(5);
            CyclicSpinBarrier outerBarrier(5);

            for(int i = 0; i < 4; ++i)
                std::thread(&process, innerBarrier, outerBarrier);

            while(true)
            {
                barrier1.wait();
                // Here, work has completed in all threads for the current loop
                // Do something synchronous with the work
                barrier2.wait();    // Allows all threads to continue looping
            }

            return 0;        
        }
        \endcode

        */
    class CyclicSpinBarrier : public AbstractBarrier
    {
    public:
        /*! Constructs a CyclicSpinbarrier with a number of threads to wait on. This number
            defaults to 2, so CyclicSpinbarriers can be constructed by calling:
            \code
            CyclicSpinBarrier();
            \endcode

            Although this is not recommended.        
        */
        CyclicSpinBarrier(size_t numThreads = 2);
        ~CyclicSpinBarrier();

        void wait() const;
    private:
        const size_t m_initial;
        mutable SpinRWMutex m_reset;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        CyclicSpinBarrier(const CyclicSpinBarrier&);
        CyclicSpinBarrier(CyclicSpinBarrier&&);
    };

}
}