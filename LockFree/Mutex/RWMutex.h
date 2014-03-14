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
// DX LockFree - RWMutex describes a multi-reader or single writer mutex-like object
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace DX {
namespace LockFree {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // RWMutex

    /*! \brief RWMutex is a general descriptor for a class of mutex-like objects that support
        multiple readers or a single writer. Unlike their Mutex counterparts, RWMutexes have two 
        different lock() and unlock() methods - one set for writers and one set for readers.
    */
    class RWMutex
    {
    public:
        RWMutex();
        virtual ~RWMutex() = 0;
        
        /*! Locks this RWMutex as a reader. Depending on the state of the object, this call may or
            may not block. 

            \note This call is gauranteed to not block if only readers have locks. Unspecified 
            otherwise        
        */
        virtual void lockReader() const = 0;
        /*! Locks this RWMutex as a writer. Depending on the state of the object, this call may or
            may not block.

            \note This call is gauranteed not to block if there are no other readers or writers that
            have the RWMutex locked. Unspecified otherwise.
        */
        virtual void lockWriter() const = 0;
        /*! Releases a reader lock on the RWMutex. This call does not block. 
           \note Assumes that this call has been properly paired with a call to lockReader()        
        */
        virtual void unlockReader() const = 0;
        /*! Releases a writerer lock on the RWMutex. This call does not block. 
            \note Assumes that this call has been properly paired with a call to lockWriter()
        */
        virtual void unlockWriter() const = 0;
    private:
        /*
            Copy and move constructors are hidden to prevent the compiler from automatically generating
            them for us. This class is currently NOT copyable or movable.
        */
        RWMutex(const RWMutex&);
        RWMutex(RWMutex&&);
    };

}
}