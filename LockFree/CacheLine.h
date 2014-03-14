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
// DX LockFree - CacheLine defines the size to use for cache padding / awareness
// Author: Eli Pinkerton
// Date: 3/14/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace DX {
namespace LockFree {

    /*!
        L1, L2, and L3 cache lines on i7s are 64 Bytes. CACHE_LINE_SIZE is used to pad classes that use
        multiple internal atomics / other variables to reduce the likelihood of cache contention and
        false sharing
    */
    #ifndef CACHE_LINE_SIZE
        #define CACHE_LINE_SIZE 64U
    #endif

}
}