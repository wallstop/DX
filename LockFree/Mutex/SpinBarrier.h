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
// DX LockFree - SpinBarrier is a Barrier based on a similar yielding mechanic to SpinYieldMutex
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AbstractBarrier.h"

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SpinBarrier

    /*! \brief SpinBarrier is designed for use to synchronize workflow between multiple threads. 
        Calls to wait() will block until some specified number of threads have called wait(), after
        which all threads will unblock and continue execution.

        \note SpinBarrier is a form of a single-use barrier. For continued use, call reset() in a 
        threadsafe fashion.    
    */    
    class SpinBarrier : public AbstractBarrier
    {
    public:
        /*! Constructs a SpinBarrier that is set up to wait on some number of threads
            \param[in] numThreads   The number of threads that are expected to use this barrier
        */
        SpinBarrier(size_t numThreads = 2);
        ~SpinBarrier();

        /*!< Blocks the current thread of execution until all other wait() calls have been made */
        void wait() const;

        /*! Resets the SpinBarrier to its initial state, for continued usage.
        
            \note reset() is NOT thread safe. 
        */
        void reset();

    private:
        size_t      m_initial;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        SpinBarrier(const SpinBarrier&);
        SpinBarrier(SpinBarrier&&);
    };

}
}