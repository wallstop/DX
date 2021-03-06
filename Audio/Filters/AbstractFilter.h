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

#include "../DXAudioExport.h"

#include <string>

namespace std 
{
    template <typename T>
    class shared_ptr;
}

namespace DX {
namespace Audio {

    class AudioPacket;

    enum FilterType
    {
        UNKNOWN                         = 0,
        FREQUENCY_TRANSFORM             = 1,
        CHANNEL_TRANSFORM               = 2,
        EFFECTS                         = 3,
        FREQUENCY_AND_CHANNEL_TRANSFORM = 4
    };

    struct DXAUDIO_EXPORT AbstractFilter
    {
        AbstractFilter(std::shared_ptr<AbstractFilter> filter, FilterType type);
        virtual ~AbstractFilter() = 0;

        virtual bool            transformPacket(const AudioPacket& in, AudioPacket& out) const = 0;
        virtual std::string     name() const = 0;
    };

#define DECLARE_FILTER(filter) static const filter s_ ## filter; 

}
}
