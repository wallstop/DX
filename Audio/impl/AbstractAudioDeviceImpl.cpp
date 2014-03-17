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

#include "AbstractAudioDeviceImpl.h"

#ifdef WIN32

#include <atlbase.h>

#endif

namespace DX {
namespace Audio {

#ifdef WIN32

    AbstractAudioDeviceImpl::AbstractAudioDeviceImpl() 
        : m_initialized(false), m_started(false), m_mmDevice(nullptr), m_client(nullptr)
    {
    }

    AbstractAudioDeviceImpl::AbstractAudioDeviceImpl(IMMDevice *device) 
        : m_mmDevice(device), m_client(nullptr), m_initialized(false), m_started(false)
    {
    }

    AbstractAudioDeviceImpl::~AbstractAudioDeviceImpl()
    {
        stop();        
        releaseDevice(m_client);
        releaseDevice(m_mmDevice);
    }

    bool AbstractAudioDeviceImpl::initialize()
    {
        // Make sure we only initialize once
        if(!m_mmDevice || m_initialized)
        {
            // Need an mmDevice handle to proceed
            return false;
        }

        LPWSTR deviceIdAsLPWSTR;
        m_mmDevice->GetId(&deviceIdAsLPWSTR);
        m_id = CW2A(deviceIdAsLPWSTR);

        // Grab our AudioClient
        int ok = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
            reinterpret_cast<void** >(&m_client));
        if(ok < 0 || !m_client)
        {
            // Unable to get an IAudioClient handle
            releaseDevice(m_client);
            return false;
        }

        ok = m_client->GetDevicePeriod(&m_referenceTime, NULL);
        if(ok < 0)
        {
            releaseDevice(m_client);
            return false;
        }

        // Setup our wave format
        WAVEFORMATEX *waveFormat = nullptr;
        ok = m_client->GetMixFormat(&waveFormat);
		if(ok < 0 || !waveFormat)
        {
            // Unable to get the device's audio format
            releaseDevice(m_client);
            CoTaskMemFree(waveFormat);
            return false;
        }

        m_audioFormat.channels			= waveFormat->nChannels;
        m_audioFormat.samplesPerSecond	= waveFormat->nSamplesPerSec;
        m_audioFormat.bitsPerBlock		= waveFormat->nBlockAlign;
        m_audioFormat.bitsPerSample		= waveFormat->wBitsPerSample;

        // Try to intialize our audio client
        ok = m_client->Initialize(AUDCLNT_SHAREMODE_SHARED, m_deviceMode, 
            0, 0, waveFormat, 0);
        CoTaskMemFree(waveFormat);  // nullptrs are ok here
        if(ok < 0)
        {
            // Unable to properly initialize IAudioClient
            releaseDevice(m_client);
            return false;
        }

        // TODO: Base querying time off of GetDevicePeriod(...)

        return true;
    }

    bool AbstractAudioDeviceImpl::start()
    {
        if(!m_initialized)
            initialize();
        if(m_client)
        {
            const int ok = m_client->Start();
            if(ok >= 0)
            {
                m_started = true;
                return true;
            }
        }
        return false;
    }

    bool AbstractAudioDeviceImpl::isStarted() const
    {
        return (m_client && m_started);
    }

    bool AbstractAudioDeviceImpl::stop()
    {
        if(m_client)
        {
            const int ok = m_client->Stop();
            if(ok >= 0)
            {
                m_started = false;
                return true;
            }
        }
        return false;
    }

    bool AbstractAudioDeviceImpl::isStopped() const
    {
        return (m_client && !m_started);
    }

    bool AbstractAudioDeviceImpl::isValid() const
    {
        return m_initialized && (isStarted() || isStopped());
    }

    std::string AbstractAudioDeviceImpl::id() const
    {
        return m_id;
    }

    AudioFormat AbstractAudioDeviceImpl::getAudioFormat() const
    {
        return m_audioFormat;
    }
    
#endif

}
}