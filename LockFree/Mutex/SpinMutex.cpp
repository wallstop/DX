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

#include "SpinMutex.h"

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinMutex impl

    SpinMutex::SpinMutex() : m_lock(false)
    {
    }

    SpinMutex::~SpinMutex()
    {
    }

    void SpinMutex::lock() const
    {
        while(m_lock.exchange(true))
        {
            // Spin out
        }
    }

    bool SpinMutex::tryLock() const
    {
        return !m_lock.exchange(true);
    }

    void SpinMutex::unlock() const
    {
        m_lock = false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinLock impl

    SpinLock::SpinLock(const SpinMutex& _mutex) : m_mutex(&_mutex)
    {
        assert(m_mutex); // We should have a handle on a valid mutex
        if(m_mutex)
            m_mutex->lock();
    }

    SpinLock::~SpinLock()
    {
        assert(m_mutex); // We should have a handle on a valid mutex
        if(m_mutex)
            m_mutex->unlock();
    }

}
}