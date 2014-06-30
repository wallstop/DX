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

#include "AudioStream.h"

#include <memory>
#include <string>

namespace DX {
namespace Audio {

    struct  AudioFormat;
    struct  AbstractFilter;
    class   AudioPacket;
    class   TaskCallback;
    class   AbstractAudioDeviceImpl;

    /*!
        \brief  AbstractAudioDevice is the base class for any system-recognized
                hardware 
    */
    class AbstractAudioDevice
    {
    public:

        virtual ~AbstractAudioDevice() = 0;

        virtual bool initialize() = 0;

        // TODO: Make these pure virtual
        virtual bool start();
        virtual bool stop();

        virtual bool isCaptureDevice() const;
        virtual bool isPlaybackDevice() const;
        virtual bool isValid() const;	

        virtual AudioFormat     getAudioFormat() const;	
        virtual AudioPacket     readFromBuffer() = 0;   
        //virtual AudioStream     readFromBuffer(TaskCallback* callback) = 0;
        virtual bool            readFromBuffer(AudioStream& out, TaskCallback* callback) = 0;
        virtual bool            writeToBuffer(const AudioPacket& in, const AbstractFilter& filter) = 0;
        virtual bool            writeToBuffer(AudioStream& in, const AbstractFilter& filter, TaskCallback* callback = nullptr) = 0;

        virtual std::string     id() const;

    protected:
        AbstractAudioDevice();		

        AbstractAudioDevice(const AbstractAudioDevice&);
        AbstractAudioDevice(AbstractAudioDevice&&);

        std::shared_ptr<AbstractAudioDeviceImpl> m_impl;

        //friend class AudioDeviceManager;
        friend class AudioDeviceManagerImpl;
    };

}
}