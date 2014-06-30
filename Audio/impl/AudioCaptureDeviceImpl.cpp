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

#include "AudioCaptureDeviceImpl.h"
#include "../AudioPacket.h"

#include <thread>

namespace DX {
namespace Audio {
	
#ifdef WIN32

    AudioCaptureDeviceImpl::AudioCaptureDeviceImpl(IMMDevice *mmDevice, IAudioCaptureClient* captureClient) 
        : AbstractAudioDeviceImpl(mmDevice), m_captureClient(captureClient)
    {
    }

    AudioCaptureDeviceImpl::~AudioCaptureDeviceImpl()
    {
        stop();
        releaseDevice(m_captureClient);
    }

    bool AudioCaptureDeviceImpl::initialize()
    {
        if(!AbstractAudioDeviceImpl::initialize())
            return false;

        m_captureClient = nullptr;

        // Try and actually get a handle to the captureClient
        int ok = m_client->GetService(__uuidof(IAudioCaptureClient),
            reinterpret_cast<void** >(&m_captureClient));
        if(ok < 0 || !m_captureClient)
        {
            // Unable to properly active our captureClient
            releaseDevice(m_captureClient);
            return false;
        }

        // still here? 
        m_initialized = true;
        return true;
    }

    bool AudioCaptureDeviceImpl::writeToBuffer(const AudioPacket&, const AbstractFilter&)
    {
        return false;
    }

    bool AudioCaptureDeviceImpl::writeToBuffer(AudioStream&, const AbstractFilter&, TaskCallback*)
    {
        return false;
    }

    AudioPacket AudioCaptureDeviceImpl::readFromBuffer()
    {
        if(!isStarted())
        {
            start();
            if(!isStarted())
                return BAD_BUFFER;
        }

        if(!m_captureClient)
            return BAD_BUFFER;

        // See if there's any data we can get
        unsigned int numFramesToRead = 0;
        int ok =  0;
        ok = m_captureClient->GetNextPacketSize(&numFramesToRead);
        //int ok = m_captureClient->GetNextPacketSize(&numFramesToRead);
        if(ok < 0)
        {
            // Unable to get the next packet size... something went wrong
            return BAD_BUFFER;
        }

        if(numFramesToRead == 0)
            return AudioPacket(m_audioFormat, 0);

        AudioByte* data = nullptr;        
        unsigned long returnFlags = 0;
        ok = m_captureClient->GetBuffer(&data, &numFramesToRead, &returnFlags, nullptr, nullptr);
        if(ok < 0)
        {
            // Something bad happened when we tried to grab the buffer
            // Releasing the buffer here may or may not be bad...
            stop();
            m_captureClient->ReleaseBuffer(numFramesToRead);
            return BAD_BUFFER;
        }

        // TODO: Some logic / error handling based off of flag return values. Skipping that for now.
        const size_t numBytesToWrite = numFramesToRead * m_audioFormat.bitsPerBlock;
        AudioPacket ret(m_audioFormat, numBytesToWrite);

        // TODO: Think about adding some kind of bulk-inserter?
        ret.assign(data, numBytesToWrite);

        ok = m_captureClient->ReleaseBuffer(numFramesToRead);
        if(ok < 0)
        {
            // Seriously? It failed here? How?
            //stop();
        }

        return ret;
    }

    bool  AudioCaptureDeviceImpl::readFromBuffer(AudioStream& out, TaskCallback* callback)
    {
        if(!isStarted())
        {
            start();
            if(!isStarted())
                return false;
        }

        if(!m_captureClient)
            return false;

        bool continueReading = true;
        const size_t maxPacketsFallback = 100000;
        size_t packetsRead = 0;
        while(continueReading)
        {
            // See if there's any data we can get
            //d::this_thread::sleep_for(std::chrono::microseconds(m_referenceTime));
            unsigned int numFramesToRead = 0;
            int ok = m_captureClient->GetNextPacketSize(&numFramesToRead);
            if(ok < 0)
            {
                // Unable to get the next packet size... something went wrong
                return false;
            }

            if(numFramesToRead == 0)
            {
                if(callback)
                    continueReading = !callback->isTaskStopped();
                continue;
            }

            AudioByte* data = nullptr;      
            numFramesToRead = 0;
            unsigned long returnFlags = 0;
            ok = m_captureClient->GetBuffer(&data, &numFramesToRead, &returnFlags, 0, 0);
            if(ok < 0 || data == nullptr)
            {
                // Something bad happened when we tried to grab the buffer
                // Releasing the buffer here may or may not be bad...
                //stop();
                m_captureClient->ReleaseBuffer(numFramesToRead);
                return false;
            }

            if(numFramesToRead == 0)
            {
                if(callback)
                    continueReading = !callback->isTaskStopped();
                //m_captureClient->ReleaseBuffer(numFramesToRead);
                continue;
            }

            // TODO: Some logic / error handling based off of flag return values. Skipping that for now.
            const size_t bytesToCopy = numFramesToRead * m_audioFormat.bitsPerBlock;
            AudioPacket ret(m_audioFormat, bytesToCopy);

            ret.assign(data, bytesToCopy);
            out.push(std::move(ret));

            ok = m_captureClient->ReleaseBuffer(numFramesToRead);
            if(ok < 0)
            {
                // Seriously? It failed here? How?
                //stop();
            }

            continueReading = (callback != nullptr ? !callback->isTaskStopped() : ++packetsRead < maxPacketsFallback);
        }

        stop();

        return true;
    }

#endif

}
}
