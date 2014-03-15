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
// First-Pass at high performance Node-type structure for singly linked list-based data structures
// Author: Eli Pinkerton
// Date: 2/26/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../CacheLine.h"

#include <atomic>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Node
    
    template <typename T>
    struct Node
    {
        Node();
        Node(T* data);
        ~Node();

        T* data;
        std::atomic<Node*> next;
        volatile char pad_[CACHE_LINE_SIZE - ((sizeof(T*) + sizeof(std::atomic<Node*>)) % CACHE_LINE_SIZE)];

    private:
        Node(const Node&);
        Node(Node&&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Queue

    template <typename T>
    class Queue
    {
    public:
        Queue();
        virtual ~Queue() = 0;

        virtual bool    isEmpty() const;
        virtual size_t  size() const;
        virtual bool    front(T& out) const = 0;
        virtual bool    pop(T& in) = 0;
        virtual void    push(const T& in) = 0;
        virtual void    push(T&& in) = 0;

        virtual void    clear() = 0;

        // blocks
        bool    operator>>(T&);
        // blocks
        Queue&  operator<<(const T&);

    protected:
        Node<T>*            m_start;
        volatile char       pad_0[CACHE_LINE_SIZE - (sizeof(Node<T>*) % CACHE_LINE_SIZE)];
        Node<T>*            m_end;
        volatile char       pad_1[CACHE_LINE_SIZE - (sizeof(Node<T>*) % CACHE_LINE_SIZE)];
        std::atomic<size_t> m_size;
        volatile char       pad_2[CACHE_LINE_SIZE - (sizeof(std::atomic<size_t>) % CACHE_LINE_SIZE)];
    private:
        Queue(const Queue&);
        Queue(Queue&&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Node impl

    template <typename T>
    Node<T>::Node() : next(nullptr), data(nullptr)
    {
    }

    template <typename T>
    Node<T>::Node(T* _data) : data(_data), next(nullptr)
    {
    }

    template <typename T>
    Node<T>::~Node()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Queue impl

    template <typename T>
    Queue<T>::Queue() : m_start(nullptr), m_end(nullptr), m_size(0)
    {
    }

    template <typename T>
    Queue<T>::~Queue()
    {
    }

    template <typename T>
    size_t Queue<T>::size() const
    {
        return m_size;
    }

    template <typename T>
    bool Queue<T>::isEmpty() const
    {
        return m_size == 0;
    }

    template<typename T>
    Queue<T>& Queue<T>::operator<<(const T& object)
    {
        push(object);
        return *this;
    }

    template <typename T>
    bool Queue<T>::operator>>(T& object)
    {
        bool inserted = false;
        do
        {
            inserted = pop(object);
        }
        while(!inserted);

        return inserted;
    }

}
}