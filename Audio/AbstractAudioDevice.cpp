/* /////////////////////////////////////////////////////////////////////////////////////////////////
    DX Audio - A high-level audio library designed for interacting easily with hardware devices
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

#include "AbstractAudioDevice.h"
#include "impl/AbstractAudioDeviceImpl.h"

namespace DX {
namespace Audio {

    AbstractAudioDevice::AbstractAudioDevice() : m_impl(nullptr)
    {
    }

    AbstractAudioDevice::~AbstractAudioDevice()
    {
    }

    bool AbstractAudioDevice::initialize()
    {
        return (m_impl && m_impl->initialize());
    }

    bool AbstractAudioDevice::start()
    {
        return (m_impl && m_impl->start());
    }

    bool AbstractAudioDevice::stop()
    {
        return (m_impl && m_impl->stop());
    }

    inline bool AbstractAudioDevice::isCaptureDevice() const
    {
        return false;
    }

    inline bool AbstractAudioDevice::isPlaybackDevice() const
    {
        return false;
    }

    bool AbstractAudioDevice::isValid() const
    {
        return (m_impl && m_impl->isValid());
    }

    AudioFormat AbstractAudioDevice::getAudioFormat() const
    {
        if(m_impl)
            return m_impl->getAudioFormat();
        return AudioFormat();
    }

    std::string AbstractAudioDevice::id() const
    {
        if(m_impl)
            return m_impl->id();
        return std::string();
    }

}
}