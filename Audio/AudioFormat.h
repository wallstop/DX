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
// DX Audio - AudioFormat defines useful sampling information for tagging relevant classes
// Author: Eli Pinkerton
// Date: 3/13/14
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DX {
namespace Audio {

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Audio Format

    typedef unsigned char AudioByte;

    /*! \brief AudioFormat is a lightweight tag structure designed to attach to things that need
        some kind of audio format information. This typically includes hardware audio devices, where
        the information is pulled from, Streams to and from devices, and AudioPackets themselves.
        Streams and AudioPackets especially can be passed around freely. Any given class accessing 
        them may want to know the format or any additional information about the data contained, as 
        otherwise, the data would be contextless and largely useless.
        
        \note This will be expanded upon to contain any further useful information as the need 
        arises. (Specifically, regarding support for platforms other than WIN32)

        \detailed This class should be able to accurately describe all common audio formats as well
        as any potential custom formats that may or may not exist. Filters should have the capability
        to adapt to any possible sampling frequency, channel count, bit depth, and sample size.
        A "typical" AudioFormat will feature something like:
        channels            = 2
        samplesPerSecond    = 44100
        bitsPerBlock        = 8
        bitsPerSample       = 32

        For a 44.1 KHz, 16-bit depth audio format    
    */
    struct AudioFormat
    {
        unsigned short  channels; /*!< Number of channels that the AudioFormat has data for */
        unsigned int    samplesPerSecond; /*!< Frequency of sound that the AudioFormat represents */
        unsigned short  bitsPerBlock; /*!< Number of bits to align a single block to */
        unsigned int    bitsPerSample; /*!< Number of bits per sample of mono data */

        bool operator==(const AudioFormat& other) const;
        bool operator!=(const AudioFormat& other) const;
    };

    /*! A default-created AudioFormat is inherintly "bad". This is also useful for compile-time sanity
        checks    
    */
    static AudioFormat BAD_FORMAT;

}
}
