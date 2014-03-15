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
// DX LockFree - SpinMutex and SpinLock are spin-based implementations of a Mutex
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../CacheLine.h"
#include "Mutex.h"

#include <atomic>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinMutex

    /*! \brief SpinMutex is a leightweight mutex class that makes use of C++11 atomics to spin out in
        active-CPU-land as opposed to the traditional method of yielding context. It's target 
        use-case is for code regions that aren't too highly contended, or for contended regions
        that execute fairly fast.

        \note SpinMutex is not a recursive mutex

        Sample of a SpinMutex protecting an int:
        \code
            mutable SpinMutex myMutex;
            int myProtectedValue; 

            void setMyValue(int newValue)
            {
                myMutex.lock();
                myProtectedValue = newValue;
                myMutex.unlock();
            }

            int getMyValue() const
            {
                int ret;
                myMutex.lock();
                ret = myProtectedValue;
                myMutex.unlock();

                return ret;
            }
            \endcode

            Please note that the above example could also be accomplished by making the type of
            myProtectedValue std::atomic<int>, but the usage still stands.
    */
    class SpinMutex : public Mutex
    {
    public:
        SpinMutex();
        virtual ~SpinMutex();

        /*! \brief Locks the mutex. Further calls to lock will block until there is a call to unlock().
            
            \note lock() is not recursive.
            \note lock() is blocking.
        */
        virtual void lock() const;

        /*! \brief Attempts to lock the mutex. tryLock() is non-blocking, meaning that regardless of 
            the state of the mutex when tryLock() is called, execution will continue.

            \note tryLock() is not recursive.
            \note tryLock() is non-blocking.
        */
        virtual bool tryLock() const;
        /*! \brief Unlocks the mutex. This call is non-blocking, because having it otherwise doesn't
            make any sense.
        */
        virtual void unlock() const; 

    protected:
        // Initial padding so we aren't overlapping some other potentially contended cache
        volatile char pad_0[CACHE_LINE_SIZE];
        mutable std::atomic<bool> m_lock;
        // And then flesh out the rest of our pad
        volatile char pad_1[CACHE_LINE_SIZE - (sizeof(std::atomic<bool>) % CACHE_LINE_SIZE)];

    private:

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinMutex(const SpinMutex&);
        SpinMutex(SpinMutex&&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    /*! \brief SpinLock is a lock-guard style class that latches onto a mutex, locking it upon creation
        and unlocking it upon destruction.

        SpinLocks are the preferred way of interacting with SpinMutex.

        \note SpinLocks do not allow for recursive locking of a SpinMutex

        Here are two examples of using SpinLocks to protect critical sections of code:
        \code

        MyClass myClass;
        SpinMutex myMutex;

        void getMyClass() const
        {
            SpinLock _lock(myMutex);
            return myClass; // After the return, _lock destroys itself, automatically unlocking myMutex
        }
        \endcode

        \code
        MyClass myClass;
        SpinMutex myMutex;

        void doSomeThings(const MyClass& other)
        {
            // The below scope is thread-safe, the rest of the function isn't
            {
                SpinLock _lock(myMutex);
                myClass = other;
            }
            // Continue doing things here - the mutex is unlocked!
        }
        \endcode
    */
    class SpinLock //: Lock
    {
    public:
        /*! \param[in] mutex The SpinMutex the lock will lock and guard
        */
        SpinLock(const SpinMutex& mutex);
        ~SpinLock();
    private:
        const SpinMutex* m_mutex;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinLock(const SpinLock&);
        SpinLock(SpinLock&&);
    };

}
}