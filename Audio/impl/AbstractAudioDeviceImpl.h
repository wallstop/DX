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

#pragma once

#include "../AbstractAudioDevice.h"
#include "../AudioFormat.h"
#include "../Tasks/TaskCallback.h"
#include "../AudioStream.h"

#ifdef WIN32

#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

#endif

namespace DX {
namespace Audio {

#ifdef WIN32

// Safely releases some IUnknown* object
#define releaseDevice(device)   \
    if((device)) { (device)->Release(); (device) = nullptr; }

    class AudioPacket;
    struct AbstractFilter;

    class AbstractAudioDeviceImpl
    {
    public:
        AbstractAudioDeviceImpl();
        AbstractAudioDeviceImpl(IMMDevice *mmDevice);
        virtual ~AbstractAudioDeviceImpl();

        // TODO: Make this templated / argumented for "role" (loopback)
        virtual bool        initialize() = 0;

        // TODO: Make start() and stop() pure virtual
        virtual bool        start();
        virtual bool        isStarted() const;
        virtual bool        stop();
        virtual bool        isStopped() const;

        virtual AudioFormat getAudioFormat() const;
        virtual AudioPacket readFromBuffer() = 0;
        virtual bool        readFromBuffer(AudioStream& out, TaskCallback* callback = nullptr) = 0;
        virtual bool        writeToBuffer(const AudioPacket& in, const AbstractFilter& filter) = 0;
        virtual bool        writeToBuffer(AudioStream& in, const AbstractFilter& filter, TaskCallback* callback) = 0;

        virtual bool        isValid() const;
        virtual std::string id() const;

    protected:
        IMMDevice*          m_mmDevice;
        IAudioClient*       m_client;

        AudioFormat         m_audioFormat;

        long long           m_referenceTime;

        bool                m_initialized;
        bool                m_started;
        std::string         m_id;
        int                 m_deviceMode;

        friend class AudioDeviceManagerImpl;

        };

#endif

}
}

