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

#include "AbstractAudioDevice.h"

namespace DX {
namespace Audio {

    class AudioCaptureDeviceImpl;
    class AudioPacket;

    class AudioCaptureDevice : public AbstractAudioDevice
    {
    public:

        AudioCaptureDevice();
        virtual ~AudioCaptureDevice();

        virtual bool        initialize();

        virtual bool        isCaptureDevice() const;

        virtual AudioPacket readFromBuffer();
        virtual bool        readFromBuffer(AudioStream& out, TaskCallback* callback);
        virtual bool        writeToBuffer(const AudioPacket& in, const AbstractFilter& filter);
        virtual bool        writeToBuffer(AudioStream& in, const AbstractFilter& filter, TaskCallback* callback);

    };

}
}