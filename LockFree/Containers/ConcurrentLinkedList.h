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

#pragma once

#include "../Mutex/SpinRWMutex.h"

// TODO: All of this + impl

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // ListNode   

    template <typename T>
    struct ListNode
    {
        ListNode();
        ~ListNode();

        T* data;
        std::atomic<ListNode*> next;
        SpinRWMutex mutex;
        volatile char pad_0 [CACHE_LINE_SIZE - ((sizeof(T*) + sizeof(std::atomic<Node*>) + sizeof(SpinRWMutex)) % CACHE_LINE_SIZE)];

    private:
        ListNode(const ListNode&);
        ListNode(ListNode&&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // LinkedList

    template <typename T>
    class ConcurrentLinkedList
    {
    public:
        ConcurrentLinkedList();
        ~ConcurrentLinkedList();

        bool    isEmpty() const;
        size_t  size() const;
        

    private:

    };

}
}


