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

#include "SpinRecursiveMutex.h"

#include <thread>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////   
    // SpinRecursiveMutex impl

    SpinRecursiveMutex::SpinRecursiveMutex() : SpinMutex(), m_owner(0), m_count(0)
    {
    }

    SpinRecursiveMutex::~SpinRecursiveMutex()
    {
    }

    void SpinRecursiveMutex::lock() const
    {
        const size_t queryingThread = std::this_thread::get_id().hash();
        #if defined _DEBUG || defined DEBUG 
            const bool wasLocked = m_lock.load();
            assert(wasLocked ? queryingThread == m_owner : m_count == 0);
        #endif
        while(m_lock.exchange(true) && queryingThread != m_owner)
        {
            // Spin out while someone else has the lock
            // TOOD: yield here?
        }

        // Here we have exclusive ownership
        SpinLock _lock(m_assignmentMutex);
        m_owner = queryingThread;
        ++m_count;
    }

    bool SpinRecursiveMutex::tryLock() const
    {
        const size_t queryingThread = std::this_thread::get_id().hash();

        const bool wasLocked = m_lock.exchange(true);
        const size_t owner = m_owner;
        if(!wasLocked || (wasLocked && queryingThread == owner))
        {
            SpinLock _lock(m_assignmentMutex);
            assert(wasLocked ? queryingThread == m_owner : m_count == 0);
            m_owner = queryingThread;
            ++m_count;
            return true;
        }
        return false;        
    }

    void SpinRecursiveMutex::unlock() const
    {
        #if defined(_DEBUG) || defined(DEBUG)
            const size_t queryingThread = std::this_thread::get_id().hash();
            assert(m_owner == queryingThread);
        #endif

        assert(m_count > 0); // Make sure unlock() isn't called more than lock()
        if(--m_count == 0)
        {
            m_lock = false;
            m_owner = 0;
        }
    }

}
}