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

#include "AudioPlaybackDeviceImpl.h"
#include "../AudioPacket.h"
#include "../Filters/AbstractFilter.h"

#include <thread>

namespace DX {
namespace Audio {

#ifdef WIN32
	
    AudioPlaybackDeviceImpl::AudioPlaybackDeviceImpl(IMMDevice* mmDevice, IAudioRenderClient* playbackClient) 
        : AbstractAudioDeviceImpl(mmDevice), m_playbackClient(playbackClient)
    {
        initialize();
    }

    AudioPlaybackDeviceImpl::~AudioPlaybackDeviceImpl()
    {
        stop();
        releaseDevice(m_playbackClient);
    }

    bool AudioPlaybackDeviceImpl::initialize()
    {
        if(!AbstractAudioDeviceImpl::initialize())
            return false;

        int ok = m_client->GetService(__uuidof(IAudioRenderClient),
            reinterpret_cast<void** >(&m_playbackClient));
        if(ok < 0)
        {
            releaseDevice(m_client);
            releaseDevice(m_playbackClient);
            return false;
        }

        // TODO: Buffer stuff. It can wait

        // still here?
        m_initialized = true;
        return true;
	}

    AudioPacket AudioPlaybackDeviceImpl::readFromBuffer() 
    {
        return BAD_BUFFER;
    }

    bool AudioPlaybackDeviceImpl::readFromBuffer(AudioStream&, TaskCallback*)
    {
        return false;
    }

    bool AudioPlaybackDeviceImpl::writeToBuffer(const AudioPacket& in, const AbstractFilter& filter)
    {
        if(!isStarted())
        {
            // Someone called this method without starting. Let's see what we can do...
            start();
            // Now, if we're still not started, bail
            if(!isStarted())
                return false;
        }


        if(!m_initialized)
        {
            // If we're not initialized here, we can't proceed. Bail out
            return false;
        }

        if(!m_client || !m_playbackClient)
        {
            // Something's wrong with our playbackClient - we don't have one!
            return false;
        }

        AudioByte *data = nullptr;
        unsigned int sizeOfBuffer = 0;
        int ok = m_client->GetBufferSize(&sizeOfBuffer);
        if(ok < 0)
        {
            // Something went wrong with getting the buffer size (...how?)
            return false;
        }

        unsigned int bytesPadded = 0;
        ok = m_client->GetCurrentPadding(&bytesPadded);
        if(ok < 0)
        {
            // Something went wrong with getting the amount ofpadding (...how?)
            return false;
        }

        if(bytesPadded > sizeOfBuffer)
        {
            // Woops, we've been writing too much!
            return false;
        }

        sizeOfBuffer -= bytesPadded;
        ok = m_playbackClient->GetBuffer(sizeOfBuffer, &data);
        if(ok < 0)
        {
            // Something went wrong with grabbing a pointer to the buffer. That's no good.
            m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0); // Should we be releasing this here? Do we care?
            return false;
        }

        // We have to do some size calculations to get the appropriately sized buffer from whatever is passed in
        const size_t outSize = determineBufferSize(in, m_audioFormat);
        AudioPacket myBuffer(m_audioFormat, outSize);
        const bool transformOk = filter.transformPacket(in, myBuffer);
        const size_t trueSize = sizeOfBuffer < outSize? sizeOfBuffer : outSize;
        if(!transformOk)
        {
            m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
            return false;
        }

        std::memcpy(data, myBuffer.data(), trueSize);

        ok = m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
        if(ok < 0)
        {
            // Some error occured, but by now, we don't care about it...
        }

        return true;
    }

    bool AudioPlaybackDeviceImpl::writeToBuffer(AudioStream& in, const AbstractFilter& filter, TaskCallback* callback)
    {
        if(!isStarted())
        {
            // Someone called this method without starting. Let's see what we can do...
            start();
            // Now, if we're still not started, bail
            if(!isStarted())
                return false;
        }

        if(!m_initialized)
        {
            // If we're not initialized here, we can't proceed. Bail out
            return false;
        }

        if(!m_client || !m_playbackClient)
        {
            // Something's wrong with our playbackClient - we don't have one!
            return false;
        }

        bool continueWriting = true;
        if(callback)
            continueWriting = !callback->isTaskStopped();

        const size_t maxPacketsFallback = 100000;   // Arbitrary
        size_t packetsWritten = 0;
        size_t emptyLoops = 0;
        const size_t maxLoops = 10; // Bailout in case we get stuck with an empty stream for too long

        while(continueWriting)
        {
            //std::this_thread::sleep_for(std::chrono::microseconds(m_referenceTime /2 ));
            unsigned int sizeOfBuffer = 0;
            int ok = m_client->GetBufferSize(&sizeOfBuffer);
            if(ok < 0)
            {
                // Something went wrong with getting the buffer size (...how?)
                return false;
            }

            unsigned int bitsPadded = 0;
            ok = m_client->GetCurrentPadding(&bitsPadded);
            if(ok < 0)
            {
                // Something went wrong with getting the amount ofpadding (...how?)
                return false;
            }

            if(bitsPadded >= sizeOfBuffer)
            {
                // Woops, we've been writing too much!
                continue;
            }

            AudioByte *data = nullptr;
            sizeOfBuffer -= bitsPadded;
            ok = m_playbackClient->GetBuffer(sizeOfBuffer, &data);
            if(ok < 0)
            {
                // Something went wrong with grabbing a pointer to the buffer. That's no good.
                m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0); // Should we be releasing this here? Do we care?
                return false;
            }

            //AudioFormat inFormat = in.getAudioFormat();
            bool gotData = true;
            //while(data && gotData && bytesWritten <= sizeOfBuffer)
            //{
            AudioPacket inPacket;
            gotData = !in.isEmpty();
            if(!gotData)
            {
                m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
                continueWriting = ++emptyLoops < maxLoops;
                continue;
            }

            gotData = in.pop(inPacket);
            if(!gotData)
            {
                // Something went wrong here
                m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
                continueWriting = ++emptyLoops < maxLoops;
                continue;
            }

            const unsigned int originalBufferSize = sizeOfBuffer;
            sizeOfBuffer *= m_audioFormat.bitsPerBlock;

            if(inPacket.byteSize() > sizeOfBuffer)
            {
                m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
                continueWriting = ++emptyLoops < maxLoops;
                continue;
            }
            emptyLoops = 0;
            const size_t outSize = determineBufferSize(inPacket, m_audioFormat);
            AudioPacket outPacket(m_audioFormat, outSize);
            const bool transformOk = filter.transformPacket(inPacket, outPacket);
            if(!transformOk)
            {
                m_playbackClient->ReleaseBuffer(sizeOfBuffer, 0);
                return false;
            }

            const unsigned int bytesToCopy = (unsigned int)(outPacket.byteSize() > sizeOfBuffer ? sizeOfBuffer : outPacket.byteSize());
            std::memcpy(data, outPacket.data(), bytesToCopy);


            // Bump the buffer pointer
            //data = data + outPacket.size();
            // bytesWritten += outPacket.size();
            continueWriting = callback ? !callback->isTaskStopped() : ++packetsWritten < maxPacketsFallback;
            //}

            ok = m_playbackClient->ReleaseBuffer(originalBufferSize, 0);
            if(ok < 0)
            {
                // Some error occured, but by now, we don't care about it...
            }

            continueWriting = (callback ? !callback->isTaskStopped() : packetsWritten < maxPacketsFallback);
        }

        stop();

        return true;

    }

#endif

}
}