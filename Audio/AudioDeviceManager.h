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

#include <vector>
#include <memory>

namespace DX {
namespace Audio {

    class AudioDeviceManagerImpl;
    class AbstractAudioDevice;
    class AudioCaptureDevice;
    class AudioPlaybackDevice;

    class AudioDeviceManager
    {
    public:
        AudioDeviceManager();
        ~AudioDeviceManager();

        bool initialize();
        bool unInitialize();

        std::vector<std::shared_ptr<AudioPlaybackDevice>>   getPlaybackDevices() const;
        std::vector<std::shared_ptr<AudioCaptureDevice>>    getCaptureDevices() const;
        std::vector<std::shared_ptr<AbstractAudioDevice>>   getAllDevices() const;
        std::shared_ptr<AudioCaptureDevice>                 getDefaultCaptureDevice() const;
        std::shared_ptr<AudioPlaybackDevice>                getDefaultPlaybackDevice() const;
        std::shared_ptr<AudioCaptureDevice>                 getDefaultPlaybackDeviceAsCaptureDevice() const;

        bool isInitialized() const;

    private:

        std::shared_ptr<AudioDeviceManagerImpl> impl;
	};

}
}