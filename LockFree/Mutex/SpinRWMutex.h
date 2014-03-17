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
// DX LockFree - SpinRWMutex is a multi reader, single writer mutex that doesn't yield
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "RWMutex.h"
#include "../CacheLine.h"
#include "SpinYieldMutex.h"

#include <atomic>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinRWMutex 

    /*! \brief SpinRWMutex is a leightweight mutex that supports multiple readers (const-only access)
        and a single writer. Supports readers up to the maximum value of size_t for your system. 

        Calls to lock() from readers will block if a writer holds the lock. Calls to lock() from
        writers will block if there are any readers or writers holding onto the lock.

        \note SpinRWMutex is favored towards writers attempting to get the lock

        \note SpinRWMutex is easiest to use with SpinRWLock. SpinRWLock lets you "set-it-and-forget-it" 
        in regards to remembering if you're a reader/writer.

        \note SpinRWMutex does *NOT* check to ensure that calls to lock/unlock are called with the same
        boolean. It does handle these cases (crash-wise), but calling lock(true) and unlock(false) from
        the same thread does not unlock the writer lock.

        \code
        mutable SpinRWMutex myRWMutex;
        MyClass myClass;

        void setMyClass(const MyClass& other)
        {
            myRWMutex.lock(true); // true indicates a writer
            myClass = other;
            myRWMutex.unlock(true); // unlock my writer reference            
        }

        MyClass getMyClass() const
        {
            // static , thread-local object so it isn't constructed every function call, only copied
            static thread_local MyClass ret;
            myRWMutex.lock(false); // false indicates reader
            ret = myClass;
            myRwMutex.unlock(false); // unlock my reader reference
            return ret;
        }
        \endcode
    */
    class SpinRWMutex : RWMutex
    {
    public:
        SpinRWMutex();
        ~SpinRWMutex();
    
        /*! Locks this SpinRWMutex as a reader. Depending on the state of the object, this call may or
            may not block.

            \note This call gauantees to not block if only readers have locks. Unspecified otherwise.        
        */
        void lockReader() const;   
        /*! Locks this SpinRWMutex as a writer. Depending on the state of the object, this call may or
            may not block.

            \note This call gauantees to not block if there are no other readers or writers currently 
            have locks. Unspecified otherwsie.        
        */
        void lockWriter() const;

        /*! Releases a reader lock on the SpinRWMutex. This call does not block.
            \note Assumes that this call has been properly paired with a call to lockReader()        
        */
        void unlockReader() const;
        /*! Releases a writer lock on the SpinRWMutex. This call does not block.
            \note Assumes that this call has been properly paired with a call to lockWriter()       
        */
        void unlockWriter() const;
    
    private:
        // Initial padding so we aren't overlapping some other potentially contended cache
        volatile char pad_0[CACHE_LINE_SIZE];
        // Keeps track of the number of readers
        mutable std::atomic<size_t> m_readerLock;
        // SpinMutex is already padded
        SpinYieldMutex m_lockMutex;
        SpinYieldMutex m_writerMutex;

        SpinRWMutex(const SpinRWMutex&);
        SpinRWMutex(SpinRWMutex&&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinRWLock
    
    /*! \brief SpinRWLock is the preferred way of interacting with a SpinRWMutex. SpinRWLock is a 
        lock-guard style class, locking the SpinRWMutex upon construction and releasing the lock upon
        destruction. SpinRWMutex remembers whether it is a reader or writer lock, and releases the 
        same kind of lock that it was constructed with.

        The big advantage of using SpinRWLocks over SpinRWMutex is the automatic unlock on destruction:
        unlike the examples provided above, no copies have to be made on a get() call

        \note SpinRWLocks do not allow for recursive writer-locks of a single SpinRWMutex

        \code
        MyClass myClass;
        mutable SpinRWMutex myRWMutex;

        MyClass getMyClass() const
        {
            SpinRWLock _lock(myRWMutex, false);
            return myClass; // No copies or thread-local storage necessary
        }

        void setMyClass(const MyClass& other)
        {
            SpinRWLock _lock(myRWMutex, true);
            myClass = other;
        }
        \endcode
    */
    class SpinRWLock
    {
    public:
        /*! \param[in] mutex The SpinRWMutex that the lock should be locking/unlocking
            \param[in] isWriter True indicates a writer lock, False indicates a reader lock
        */
        SpinRWLock(const SpinRWMutex& mutex, bool isWriter); 
        ~SpinRWLock();
    private:
        bool isWriter;
        const SpinRWMutex* m_lock;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinRWLock(const SpinRWLock&);
        SpinRWLock(SpinRWLock&&);
    };

}
}