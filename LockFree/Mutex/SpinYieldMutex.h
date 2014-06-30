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
// DX LockFree - SpinYieldMutex attempts to spin and yields then yields if the lock was not obtained
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SpinMutex.h"

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinYieldMutex

    // Arbitrary for now, best results TBD
    #ifndef DEFAULT_YIELD_TICKS
        #define DEFAULT_YIELD_TICKS 10
    #endif

    class SpinYieldMutex : public SpinMutex
    {
    public:
        /*! Default constructor
            \param[in] maxYieldTicks    Number of spins until the mutex yields
        */
        SpinYieldMutex(const size_t maxYieldTicks = DEFAULT_YIELD_TICKS);
        ~SpinYieldMutex();

        /*!< Calls to lock() will block until no other thread has ownership of the mutex. */
        void lock() const;
        /*!< Calls to tryLock() will not block. Ownership of the mutex is attempted. */
        bool tryLock() const;
        /*!< Non-blocking. Calls to unlock() will relinquish ownership of the mutex. */
        void unlock() const;
    private:
        const   size_t m_maxYieldTicks;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinYieldMutex(const SpinYieldMutex&);  // Do not use
        SpinYieldMutex(SpinYieldMutex&&);       // Do not use
    };

}
}