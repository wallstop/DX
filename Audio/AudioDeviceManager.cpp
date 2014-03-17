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

#include "AudioDeviceManager.h"
#include "impl/AudioDeviceManagerImpl.h"

#include "AudioCaptureDevice.h"
#include "AudioPlaybackDevice.h"

namespace DX {
namespace Audio {
    AudioDeviceManager::AudioDeviceManager()
    {
        impl = std::make_shared<AudioDeviceManagerImpl>();
    }

    AudioDeviceManager::~AudioDeviceManager()
    {
        unInitialize();
    }

    bool AudioDeviceManager::initialize()
    {
        return (impl && impl->initialize());
    }

    bool AudioDeviceManager::unInitialize()
    {
        return (impl && impl->unInitialize());
    }

    bool AudioDeviceManager::isInitialized() const
    {
        return (impl && impl->isInitialized());
    }
    
    std::vector<std::shared_ptr<AudioPlaybackDevice>> AudioDeviceManager::getPlaybackDevices() const
    {
        if(impl)
            return impl->getPlaybackDevices();
        return std::vector<std::shared_ptr<AudioPlaybackDevice>>();
    }

    std::vector<std::shared_ptr<AudioCaptureDevice>> AudioDeviceManager::getCaptureDevices() const
    {
        if(impl)
            return impl->getCaptureDevices();
        return std::vector<std::shared_ptr<AudioCaptureDevice>>();
    }

    std::vector<std::shared_ptr<AbstractAudioDevice>> AudioDeviceManager::getAllDevices() const
    {
        if(impl)
            return impl->getAllDevices();
        return std::vector<std::shared_ptr<AbstractAudioDevice>>();
    }

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManager::getDefaultCaptureDevice() const
    {
        if(impl)
            return impl->getDefaultCaptureDevice();
        return std::shared_ptr<AudioCaptureDevice>();
    }

    std::shared_ptr<AudioPlaybackDevice> AudioDeviceManager::getDefaultPlaybackDevice() const
    {
        if(impl)
            return impl->getDefaultPlaybackDevice();
        return std::shared_ptr<AudioPlaybackDevice>();
    }

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManager::getDefaultPlaybackDeviceAsCaptureDevice() const
    {
        if(impl)
            return impl->getDefaultPlaybackDeviceAsCaptureDevice();
        return std::shared_ptr<AudioCaptureDevice>();
    }

}
}
