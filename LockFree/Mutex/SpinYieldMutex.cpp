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

#include "SpinYieldMutex.h"

#include <thread>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinYieldMutex impl

    SpinYieldMutex::SpinYieldMutex(const size_t maxYieldTicks) : SpinMutex(), m_maxYieldTicks(maxYieldTicks)
    {
    }

    SpinYieldMutex::~SpinYieldMutex()
    {
    }

    void SpinYieldMutex::lock() const
    {
        size_t numTries = 0;
        while(m_lock.exchange(true))
        {
            if(++numTries >= m_maxYieldTicks)   // >= just for sanity
            {
                numTries = 0;
                std::this_thread::yield();
            }
        }
    }

    bool SpinYieldMutex::tryLock() const
    {
        return !m_lock.exchange(true);
    }

    void SpinYieldMutex::unlock() const
    {
        m_lock = false;
    }

}
}