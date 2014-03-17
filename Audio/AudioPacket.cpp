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

// Get rid of std::copy warnings. We know what we're doing (hopefully)
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 
#endif

#include "AudioPacket.h"

#include <assert.h>
#include <stdexcept>

namespace DX {
namespace Audio {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Sample impl

    AudioSample::AudioSample() : m_memory(nullptr)
    {
    }

    AudioSample::AudioSample(const AudioFormat& format, AudioByte* memory) : m_audioFormat(format), m_memory(memory)
    {
    }

    AudioSample::AudioSample(const AudioSample& copy)
    {
        m_memory = copy.m_memory;
        m_audioFormat = copy.m_audioFormat;
    }

    AudioSample::AudioSample(AudioSample&& move)
    {
        /*
            move's pointer should NOT be nulled out, but even if it is, as long as the std::move()
            isn't being called explicitly, but rather through compiler copy elision, this shoullllld
            be ok, as it'll be moving from somewhere into a buffer, after which, who cares about the source?
            I don't.

            // TODO: Ensure this doesn't do strange things with move (like assign a nullptr)
        */
        m_memory = move.m_memory;                                     
        m_audioFormat = std::move(move.m_audioFormat);
    }

    AudioSample& AudioSample::operator=(const AudioSample& copy)
    {
        // This is a bug - 2 channels, 2 byte will auto convert to 4 channels, 1 byte... (and similar)
        // TODO: better copying methodology

        assert(m_memory != nullptr);
        assert(copy.m_memory != nullptr);
        assert(m_audioFormat.channels == copy.m_audioFormat.channels);
        assert(m_audioFormat.bitsPerSample == copy.m_audioFormat.bitsPerSample);

        std::copy(copy.m_memory, copy.m_memory 
            + (copy.m_audioFormat.bitsPerSample / 8 * copy.m_audioFormat.channels) , m_memory);
        return *this;
    }

    AudioSample::~AudioSample()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // AudioPacket impl

    AudioPacket::AudioPacket(size_t size) : m_size(size), m_maxSize(size), m_memory(nullptr)
    {
        if(m_size > 0)
            m_memory = std::unique_ptr<AudioByte[]>(new AudioByte[m_size]);
    }

    AudioPacket::AudioPacket(const AudioFormat& format, size_t size) 
        : m_size(size), m_maxSize(size), m_memory(nullptr), m_format(format)
    {
        if(m_size > 0)
            m_memory = std::unique_ptr<AudioByte[]>(new AudioByte[m_size]);
    }

    AudioPacket::AudioPacket(const AudioPacket& copy) 
        : m_size(copy.m_size), m_maxSize(copy.m_size), m_memory(nullptr), m_format(copy.m_format)
    {
        assert(copy.m_size > 0);
        if(copy.m_size > 0)
        {
            m_memory = std::unique_ptr<AudioByte[]>(new AudioByte[copy.m_size]);
            assign(copy.m_memory.get(), copy.m_size); 
        }
        assert(m_memory.get() != nullptr);
    }

    AudioPacket::AudioPacket(AudioPacket&& move) : m_size(0), m_memory(nullptr)
    {
        // rely on our operator=(AudioPacket&&)
        *this = move;
    }

    AudioPacket::~AudioPacket()
    {
    }

    AudioSample AudioPacket::operator[](size_t index)
    {
        assert(m_memory.get() != nullptr);
        // Transform our byte index into a AudioSample index (sizeof(AudioSample) == bitsPerBlocK)
        const size_t realIndex = index * m_format.bitsPerBlock;
        if(realIndex >= m_size)
            throw std::out_of_range("");

        return AudioSample(m_format, &m_memory[realIndex]);
    }

    const AudioSample AudioPacket::at(size_t index) const
    {
        assert(m_memory.get() != nullptr);
        // Transform our byte index into a AudioSample index (sizeof(AudioSample) == bitsPerSample / 8 * channels)
        const size_t realIndex = index * m_format.bitsPerSample / 8 * m_format.channels; 
        if(realIndex >= m_size)
            throw std::out_of_range("");

        return AudioSample(m_format, &m_memory[realIndex]);
    }

    AudioFormat AudioPacket::getAudioFormat() const
    {
        return m_format;
    }

    void AudioPacket::setAudioFormat(const AudioFormat& format)
    {
        m_format = format;
    }

    bool AudioPacket::isValid() const
    {
        // An invalid state is where we don't have any audio (nullptr) or our format isn't set
        return (m_memory.get() != nullptr && m_format != AudioFormat());
    }

    AudioPacket& AudioPacket::operator=(const AudioPacket& copy)
    {
        assert(copy.m_size > 0);
        // Check to see if we have to do any resizing (expensive)
        if(copy.m_size >= m_maxSize)
        {
            // operator= on unique_ptr takes care of deletion for us
            m_memory = std::unique_ptr<AudioByte[]>(new AudioByte[copy.m_size]);
            m_size = copy.m_size;
        }
        m_format = copy.m_format;
        assign(copy.m_memory.get(), copy.m_size);

        return *this;
    }

    AudioPacket& AudioPacket::operator=(AudioPacket&& move)
    {
        m_size = move.m_size;
        m_maxSize = move.m_maxSize;
        m_format = move.m_format;
        m_memory = std::move(move.m_memory);
        move.m_memory = nullptr;
        move.m_size = 0;
        move.m_maxSize = 0;
        move.m_format = AudioFormat();
        return *this;
    }

    AudioByte* AudioPacket::data()
    {
        return m_memory.get();
    }

    const AudioByte* AudioPacket::data() const
    {
        return const_cast<const AudioByte*>(m_memory.get());
    }

    // Useful for bulk inserts
    void AudioPacket::assign(const void* data, size_t numBytes, size_t offset)
    {
        assert(m_memory.get());
        assert(m_maxSize >= numBytes + offset);
        std::copy(reinterpret_cast<const AudioByte*>(data), 
            reinterpret_cast<const AudioByte*>(data) + numBytes, m_memory.get() + offset);
    }

    void AudioPacket::assign(std::unique_ptr<AudioByte[]>&& data)
    {
        m_memory = std::move(data);
    }

    size_t AudioPacket::byteSize() const
    {
        return m_size;
    }

    size_t AudioPacket::maxSize() const
    {
        return m_maxSize;
    }

    size_t AudioPacket::numSamples() const
    {
        return (m_size / (m_format.bitsPerSample / 8 * m_format.channels));
    }

    size_t determineBufferSize(const AudioPacket& in, const AudioFormat& outFormat)
    {
        assert(in.isValid());
        assert(outFormat != AudioFormat());

        #if defined _DEBUG || defined DEBUG
            const unsigned int numInSamples = in.getAudioFormat().samplesPerSecond;
            const unsigned int numOutSamples = outFormat.samplesPerSecond;

            const unsigned short bitsPerInSample = in.getAudioFormat().bitsPerSample / 8 * in.getAudioFormat().channels;
            const unsigned short bitsPerOutSample = outFormat.bitsPerSample / 8 * outFormat.channels;

            const double ratio = (double(numOutSamples) / double(numInSamples)) * (double(bitsPerOutSample) / double(bitsPerInSample));

            return size_t(ratio * in.byteSize());

        #else
            return size_t(double(outFormat.samplesPerSecond)    / double(in.getAudioFormat().bitsPerSample / 8 * in.getAudioFormat().channels) 
                        * double(outFormat.bitsPerBlock)        / double(outFormat.bitsPerSample / 8 * outFormat.channels)
                        * double(in.byteSize()));
        #endif
    }

}
}
