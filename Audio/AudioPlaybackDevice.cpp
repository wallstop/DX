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

#include "AudioPlaybackDevice.h"
#include "impl/AudioPlaybackDeviceImpl.h"
#include "AudioPacket.h"

namespace DX {
namespace Audio {

    AudioPlaybackDevice::AudioPlaybackDevice() : AbstractAudioDevice()
    {
    }

    AudioPlaybackDevice::~AudioPlaybackDevice()
    {
    }

    bool AudioPlaybackDevice::initialize()
    {
        return (m_impl && m_impl->initialize());
    }

    inline bool AudioPlaybackDevice::isPlaybackDevice() const
    {
        return true;
    }

    bool AudioPlaybackDevice::writeToBuffer(const AudioPacket& in, const AbstractFilter& filter)
    {
        return (m_impl && m_impl->writeToBuffer(in, filter));
    }

    bool AudioPlaybackDevice::writeToBuffer(AudioStream& in, const AbstractFilter& filter, TaskCallback* callback)
    {
        return (m_impl && m_impl->writeToBuffer(in, filter, callback));
    }

    AudioPacket AudioPlaybackDevice::readFromBuffer()
    {
        if(m_impl)
            return m_impl->readFromBuffer();
        return BAD_BUFFER;
    }

    bool AudioPlaybackDevice::readFromBuffer(AudioStream& out, TaskCallback* callback)
    {
        return (m_impl && m_impl->readFromBuffer(out, callback));
    }

}
}