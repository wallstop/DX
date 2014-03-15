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
// DX LockFree - SpinRecursiveMutex allows for a single thread to continue locking a mutex recursively
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../CacheLine.h"
#include "SpinMutex.h"

#include <atomic>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinRecursiveMutex

    /*! \brief SpinRecursiveMutex allows for a single thread to continue call lock() in a non-blocking
        fashion, assuming they are the owner of the lock. Typical use case is a public function that
        requires a lock on some member data, but has to recursively call itself for proper execution.

        \code

        SpinRecursiveMutex  m_dataMutex;
        int                 m_guardedData;

        int publicMemberFunction(int n)
        {
            SpinLock _lock(m_dataMutex);
            --m_guardedData;

            if( n <= 0)
                return 0;

            return n + publicMemberFunction(n - 1);        
        }

        \encode
    
    */
    class SpinRecursiveMutex : public SpinMutex
    {
    public:
        SpinRecursiveMutex();
        ~SpinRecursiveMutex();

        /*! Locks the SpinRecursive mutex for a particular thread, if locked. If locked and the 
            calling thread is the same as the locking thread, updates the count and continues to 
            lock. 

            \note Calls to lock() from a thread other than the owning thread will block until 
            the owning thread has unlocked all holds (un-recursed fully)        
        */
        void lock() const;

        /*! tryLock() attempts to gain a lock on the mutex. Try lock is identical to lock(), 
            except that it is non-blocking.

            \note Use tryLock() sparingly, as the non-blocking nature of it makes it tricky
            to gaurantee the state of a mutex.
        */
        bool tryLock() const;

        /*! Unlocks the SpinRecursiveMutex. For every call to lock(), a matching call must be
            made to unlock() to ensure a consistent mutex state.        
        */
        void unlock() const;

    private:
        mutable SpinMutex m_assignmentMutex;    // Potentially unecessary
        mutable std::atomic<size_t> m_count;
        mutable std::atomic<size_t> m_owner;
        volatile char pad_0[CACHE_LINE_SIZE - ((sizeof(std::atomic<size_t>) + sizeof(std::atomic<bool>)) % CACHE_LINE_SIZE)];

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinRecursiveMutex(const SpinRecursiveMutex&);
        SpinRecursiveMutex(SpinRecursiveMutex&&);
    };

}
}