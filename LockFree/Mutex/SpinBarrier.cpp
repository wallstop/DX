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

#include "SpinBarrier.h"
#include "SpinYieldMutex.h" // For the DEFAULT_YIELD_TICKS definition

#include <thread>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinBarrier impl

    SpinBarrier::SpinBarrier(size_t numThreads) : AbstractBarrier(numThreads), m_initial(numThreads)
    {
    }

    SpinBarrier::~SpinBarrier()
    {
    }

    void SpinBarrier::wait() const
    {
        assert(m_count > 0); // wait called too many times
        if(m_count > 0)
            --m_count;

        // TODO: Base this lock off of SpinYieldMutex
        size_t currentTicks = 0;
        while(m_count > 0)
        {
            if(++currentTicks > DEFAULT_YIELD_TICKS)
            {
                std::this_thread::yield();
                currentTicks = 0;
            }
        }
    }

    void SpinBarrier::reset()
    {
        m_count = m_initial;
    }

}
}