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

#include "WFABF.h"
#include "../AudioPacket.h"

#include <string>

#pragma warning(push)
#pragma warning(disable : 4244) // Get rid of the pesky "double to size_t" conversion warning

namespace DX {
namespace Audio {

    DECLARE_FILTER(WFABF)

    WFABF::WFABF() : AbstractFilter(FREQUENCY_TRANSFORM)
    {
    }

    WFABF::~WFABF()
    {
    }

    bool WFABF::transformPacket(const AudioPacket& in, AudioPacket& out) const
    {
        // We were given empty buffers :(
        if(in.byteSize() == 0 || out.byteSize() == 0)
            return false;

        const double ratio = double(in.getAudioFormat().samplesPerSecond) / double(out.getAudioFormat().samplesPerSecond);
        if(ratio == 1.)
        {
            out.assign(in.data(), in.byteSize());
            return true;
        }

        const size_t maxSamples = out.numSamples();

        for(size_t i = 0; i < maxSamples; ++i)
            out[i] = in.at(size_t(double(i) * ratio));

        return true;
    }

    std::string AbstractFilter::name() const
    {
        static const std::string name("wallstop's Fast and Bad Filter.");
        return name;
    }

}
}

#pragma warning(pop)