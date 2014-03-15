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


#include "SpinRWMutex.h"

#include <thread>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinRWMutex impl

    SpinRWMutex::SpinRWMutex() : m_readerLock(0)
    {
    }

    SpinRWMutex::~SpinRWMutex()
    {
    }

    void SpinRWMutex::lockReader() const
    {
        SpinLock _lock(m_lockMutex);

        SpinLock _readLock(m_writerMutex);
        ++m_readerLock;
    }

    void SpinRWMutex::lockWriter() const
    {
        SpinLock _lock(m_lockMutex);

        m_writerMutex.lock();
        while(m_readerLock > 0) 
        {
            std::this_thread::yield();
        }       
    }

    void SpinRWMutex::unlockReader() const
    {
        assert(m_readerLock > 0); // unlock called too many times
        if(m_readerLock > 0)
            --m_readerLock;
    }

    void SpinRWMutex::unlockWriter() const
    {
        m_writerMutex.unlock();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinRWLock impl

    SpinRWLock::SpinRWLock(const SpinRWMutex& _mutex, bool _writer) 
        : m_lock(&_mutex), isWriter(_writer)
    {
        assert(m_lock); // We should have a handle on a valid mutex
        if(m_lock)
        {
            if(isWriter)
                m_lock->lockWriter();
            else
                m_lock->lockReader();
        }
    }

    SpinRWLock::~SpinRWLock()
    {
        assert(m_lock); // We should have a handle on a valid mutex
        if(m_lock)
        {
            if(isWriter)
                m_lock->unlockWriter();
            else
                m_lock->unlockReader();
        }
    }

}
}