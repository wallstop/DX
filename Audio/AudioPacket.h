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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// DX Audio - AudioPacket defines chunks of audio data for direct buffer I/O. AudioFormat tagged.
// Author: Eli Pinkerton
// Date: 3/13/14
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AudioFormat.h"

#include <memory>

namespace DX {
namespace Audio {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // AudioSample

    /*! \brief AudioSamples are byte packages of an individual sample of some audio waveform. The 
        actual bitwise-makeup of any given sample is defined by an AudioFormat. AudioSamples 
        should *NEVER* be orphaned from an AudioPacket - they hold simple references to memory
        held by an AudioPacket, and have undefined (bad!) behavior if kept around past the lifetime
        of their parent AudioPacket.

        AudioSamples, for most cases, should never be directly accessed. Their primary purpose is to
        provide a nice abstraction for Filters (DSPs) converting waveforms   
    */
    class AudioSample
    {
    public:
        AudioSample(); /*!< Default constructor, initializes empty AudioSample */

        /*! Given an AudioFormat and a pointer to some memory, outlines a single sample starting at 
            at that point 
        */
        AudioSample(const AudioFormat& format, AudioByte* memory); 
        AudioSample(const AudioSample& copy); /*!< Copy constructor, performs a shallow copy */
        AudioSample(AudioSample&& move); /*!< Move constructor, should not invalidate move's data */
        ~AudioSample(); /*!< Does NOT clean up memory */

        /*! \brief  Copies the data that one sample points at to the AudioSample defined by this.
            \note   This is the only method that AudioSamples have that allows for memory modification. 
            \note   This method DOES NOT PERFORM ANY TRANSFORMS. If the channels & bitsPerSample of the
                    corresponding AudioSamples do not align, then this will result in BAD DATA
        */
        AudioSample& operator=(const AudioSample& copy);

    private:
        /*! Ensure that the compiler doesn't do rval assigns*/
        AudioSample& operator=(AudioSample&& move); 

        AudioFormat     m_audioFormat;
        AudioByte*      m_memory;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // AudioPacket

    // TODO: Handle endianess
    // TODO: Think about adding some kind of framing information?

    /*! \brief AudioPacket is designed to hold a chunk of AudioSamples for hardware waveform IO.
        The simplest way of thinking about the class is as an array of AudioSample. AudioPackets
        manage their own memory and have many useful functions, such as raw data access as well as
        array-like access.

        \note operator[] and at() both return AudioSamples as opposed to pure AudioBytes. This is
        for ease of AudioFilter / DSP creation
    */
    class AudioPacket
    {
    public:

        //! Defines the default size for AudioPackets
        #ifndef DEFAULT_PACKET_SIZE
            #define DEFAULT_PACKET_SIZE 0
        #endif
        
        /*! \brief  Constructs an AudioPacket with DEFAULT_PACKET_SIZE number of AudioBytes.
            \note   The initial AudioFormat for an AudioPacket constructed this way will be invalid
            \note   No memory will be allocated if a size of 0 is provided
        */
        AudioPacket(size_t size = DEFAULT_PACKET_SIZE);

        /*! \brief  Constructs an AudioPacket based off of a given AudioFormat and a size representing
            the number of bytes that the AudioPacket should hold. 

            This is the preferred method of creating an AudioPacket. When typically creating an 
            AudioPacket, both the AudioFormat and byte amount should be readily available.

            \note   The size represents the number of AudioBytes, not AudioSamples        
        */
        AudioPacket(const AudioFormat& format, size_t size = DEFAULT_PACKET_SIZE);
        AudioPacket(const AudioPacket& copy); /*!< Performs a deep copy on another AudioPacket */
        AudioPacket(AudioPacket&& move); /*!< Transfers ownership of AudioPacket resources */
        ~AudioPacket(); /*!< Fully destroys all resources held by the AudioPacket*/
  
        /*! Returns an AudioSample that holds references to memory held by this AudioPacket. Typically
            operator[] would return a T&. However, no AudioSamples are held internally by an AudioPacket,
            only AudioByte. So, the returned AudioSample acts as a nice reference adapter for a group of
            AudioBytes        
        */
        AudioSample         operator[](size_t index);
        /*! Same as operator[], except allows for explicitly const access */
        const AudioSample   at(size_t index) const;
        /*! Transfers ownership of resources from some AudioPacket to this one */
        AudioPacket&        operator=(AudioPacket&& move);
        /*! Performs a deep copy of AudioPacket resources */
        AudioPacket&        operator=(const AudioPacket& copy);

        AudioFormat         getAudioFormat() const; /*!< Accessor for the AudioPacket's AudioFormat */
        void                setAudioFormat(const AudioFormat&); /*!< Mutator for the AudioPacket's AudioFormat */

        /*! \return The current number of AudioBytes that an AudioPacket thinks are valid.
        */
        size_t              byteSize() const;
        /*! AudioPackets have the potential to be resized multiple times. While this should not be the case for
            performant applications, AudioPackets will not new memory if they do not have to. As such, there is
            the potential for the maximum number of AudioBytes that an AudioPacket has allocated to not be the same
            as the number of AudioBytes that are valid.

            \return The current number of AudioBytes that an AudioPacket can interact with before performing
            an allocation
        */
        size_t              maxSize() const;
        /*! \return A best-guess estimate of the number of AudioSamples that this AudioPacket currently contains.

            \note Due to the nature of AudioSamples, this method should provide reliable data, assuming no
            hardware errors occured in the formation of the AudioPacket.
        */
        size_t              numSamples() const;
        /*! \return True if the AudioPacket has a valid AudioFormat attached to it AND contains some number of
            AudioBytes. False otherise        
        */
        bool                isValid() const;

        /*! \brief  Returns a pointer to the raw data held by the AudioPacket.
            \note   This should be used incredibly sparingly. This method is largely for internal use only.        
        */
        AudioByte*          data();
        /*! \brief  Returns a read-only pointer to the raw data held by the AudioPacket.
            \note   This should be used incredibly sparingly. This method is largely for internal use only.
        */
        const AudioByte*    data() const;

        /*! \brief Assigns a chunk of memory to some location specified by offSet in the AudioPacket

            \param[in] data     A pointer to the data to read from
            \param[in] numBytes The number of bytes to copy
            \param[in] offset   The byte offset in the AudioPacket's internal  memory to copy to

            \note This method should be used incredibly sparingly, if at all. This method is largely for
            internal use only.        
        */
        void                assign(const void* data, size_t numBytes, size_t offset = 0);

        /*! \brief Moves a unique_pointer representing some valid memory containing AudioBytes

            \note This method should be used incredibly sparingly, if at all. This method is largely for 
            internal use only.

            \note This method WILL ONLY UPDATE THE INTERNAL MEMORY POINTER. If this method is provided
            a unique_ptr that doesn't match up exactly to the size and maxSize values of this AudioPacket
            the AudioPacket's integrity will be incredbily compromised.        
        */
        void                assign(std::unique_ptr<AudioByte[]>&& data);

    private:
        size_t                          m_size;    
        size_t                          m_maxSize;
        std::unique_ptr<AudioByte[]>    m_memory;
        AudioFormat                     m_format;

    };

    /*! Given a valid AudioPacket and a format for which it needs to be output to, determines the number of AudioBytes
        that will be in the out AudioPacket

        \param[in] in           A valid AudioPacket from a source
        \param[in] outFormat    The format that the caller wishes to transform the in AudioPacket to

        \note Assumes a valid AudioPacket for in. Will provide garbage values if in is not valid.    
    */
    size_t  determineBufferSize(const AudioPacket& in, const AudioFormat& outFormat);

    /*! Static declaration of an AudioPacket both for compile-time sanity checks and for use elsewhere in the system.

        \note   Comparing some AudioPacket != BAD_BUFFER should result in the same value as AudioPacket.isValid()    
    */
    static const AudioPacket BAD_BUFFER;

}
}