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
// First-Pass at high performance single-reader, single-writer queue, "Stream"
// Author: Eli Pinkerton
// Date: 2/26/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AbstractQueue.h"

#include <new>

namespace DX {
namespace LockFree {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // ConcurrentStream 

    template <typename T>
    class ConcurrentStream : public Queue<T>
    {
    public:
        ConcurrentStream();
        ConcurrentStream(const ConcurrentStream&);
        ConcurrentStream(ConcurrentStream&&);
        ~ConcurrentStream();

        bool    isEmpty() const;
        size_t  size() const;
        bool    front(T& out) const;
        bool    pop(T& out);
        void    push(const T& in);
        void    push(T&& moveIn);

        void    clear();
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // ConcurrentStream impl

    template <typename T>
    ConcurrentStream<T>::ConcurrentStream() : Queue<T>()
    {
        m_start = new Node<T>();
        m_end = m_start;
        assert(m_start != nullptr);
    }

    template <typename T>
    ConcurrentStream<T>::ConcurrentStream(const ConcurrentStream& copy)
    {
        m_start = new Node<T>();
        assert(m_start != nullptr);
        assert(copy.m_start != nullptr);

        Node<T>* currentNode = copy.m_start->next;
        while(currentNode != nullptr)
        {
            assert(currentNode->data != nullptr);
            push(*(currentNode->data));      
            currentNode = currentNode->next;
        }
    }

    template <typename T>
    ConcurrentStream<T>::ConcurrentStream(ConcurrentStream&& move)
    {
        m_start = new Node<T>();
        m_start->next = move.m_start->next;
        move.m_start->next = nullptr;
        if(move.m_end == move.m_start)
        {
            m_end = m_start;
        }
        else
        {
            move.m_end = move.m_start;
            m_end = move.m_end;
        }
        move.m_size = 0;

        assert(m_start != nullptr);
        assert(m_end != nullptr);
    }

    template <typename T>
    ConcurrentStream<T>::~ConcurrentStream()
    {
        clear();
        
        delete m_start->data;
        m_start->data = nullptr;
        delete m_start;
        m_start = nullptr;
    }

    template <typename T>
    void ConcurrentStream<T>::clear()
    {
        while(m_start->next.load() != nullptr)
        {
            Node<T>* currentNode = m_start;
            m_start = currentNode->next.load();
            if(currentNode->data != nullptr)
            {
                delete currentNode->data;
                currentNode->data = nullptr;
            }
            delete currentNode;
            currentNode = nullptr;
        }
    }

    template <typename T>
    bool ConcurrentStream<T>::isEmpty() const
    {
        return m_size == 0;
    }

    template <typename T>
    size_t ConcurrentStream<T>::size() const
    {
        return m_size;
    }

    template <typename T>
    bool ConcurrentStream<T>::front(T& out) const
    {
        assert(m_start);
        if(m_start->next.load() == nullptr)
            return false;
        if(m_start->next.load()->data == nullptr)
            return false;

        out = *(m_start->next.load()->data);
        return true;
    }

    template <typename T>
    bool ConcurrentStream<T>::pop(T& out)
    {
        assert(m_start != nullptr);

        Node<T>* newStart = m_start->next.load();
        if(newStart == nullptr)
            return false;

        Node<T>* oldStart = m_start;
        m_start = newStart;

        assert(m_start->data != nullptr);
        out = std::move(*(m_start->data));
        assert(m_size > 0);
        --m_size;

        delete oldStart->data;
        // Only null out what used to be there in DEBUG mode
        #if defined _DEBUG || defined DEBUG 
            oldStart->data = nullptr;
        #endif
        delete oldStart;
        #if defined _DEBUG || defined DEBUG
            oldStart = nullptr;
        #endif

        return true;
    }

    template <typename T>
    void ConcurrentStream<T>::push(const T& in)
    {
        assert(m_end != nullptr);

        Node<T>* temp = new (std::nothrow) Node<T>(new (std::nothrow) T(in));
        assert(temp != nullptr);
        assert(temp->data != nullptr);

         /*
            Increment size before updating the Node's next ptr so we never have 
            the case of the queue reporting a size smaller than it is - bigger
            is ok.
        */
        ++m_size;
        m_end->next = temp;
        m_end = temp;
    }

    template <typename T>
    void ConcurrentStream<T>::push(T&& moveIn)
    {
        assert(m_end != nullptr);

        Node<T>* temp = new (std::nothrow) Node<T>(new (std::nothrow) T(moveIn));
        assert(temp != nullptr);
        assert(temp->data != nullptr);

         /*
            Increment size before updating the Node's next ptr so we never have 
            the case of the queue reporting a size smaller than it is - bigger
            is ok.
        */
        ++m_size;
        m_end->next = temp;
        m_end = temp;
    }

}
}