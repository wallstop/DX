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

#include "CyclicSpinBarrier.h"

#include <thread>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // CyclicSpinBarrier impl

    CyclicSpinBarrier::CyclicSpinBarrier(size_t numThreads) 
        : AbstractBarrier(numThreads), m_initial(numThreads)
    {
    }

    CyclicSpinBarrier::~CyclicSpinBarrier()
    {
    }

    void CyclicSpinBarrier::wait() const
    {
        // Calls to wait while the counter is still resetting will block here
        m_reset.lockReader();
        assert(m_count > 0); // wait called too many times
        const size_t count = m_count > 0 ? --m_count : 0;   // Snapshot what the count was when we enter
        {
            if(count == 0)
            {
                /*
                    Relinquish our hold on the reentrant lock, grab one as a writer.
                    This will block until every other thread waiting has finished their wait
                */
                m_reset.unlockReader();
                m_reset.lockWriter();
            }
            while(m_count > 0)
            {
                std::this_thread::yield();
            }
            if(count == 0)
            {
                m_count = m_initial;
                m_reset.unlockWriter();
            }
        }
        // The "resetter" has already unlocked their hold on the reentrant lock
        if(count != 0)  // Rely on local storage so we don't get multiple unlockers
            m_reset.unlockReader();
    }

}
}