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
// DX LockFree - First pass at an RAII object for locking and unlocking a std::mutex
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../CacheLine.h"

namespace std
{
    class mutex;
}

namespace DX {
namespace LockFree {

    /*! \brief StdLock is a RAII lockguard for some std::mutex which acquires a lock on it upon
        creation, and releases the lock upon destruction.
    */
    class StdLock
    {
    public:
        StdLock(std::mutex& _mutex);
        ~StdLock();
    private:
        std::mutex* m_mutex;

        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        StdLock(const StdLock&);
        StdLock(StdLock&&);
    };

}
}