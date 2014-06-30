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

#include "../AudioDeviceManager.h"

#include <set>

#ifdef WIN32

#include <mmdeviceapi.h>

#endif

namespace DX {
namespace Audio {

#ifdef WIN32

    class AbstractAudioDevice;
    struct EDataFlow;

    class AudioDeviceManagerImpl
    {
    public:
        AudioDeviceManagerImpl();
        ~AudioDeviceManagerImpl();

        bool initialize();
        bool unInitialize();

        bool isInitialized() const;

        void enumerateAllDevices();
        void enumeratePlaybackDevices();
        void enumerateCaptureDevices();

        std::vector<std::shared_ptr<AudioCaptureDevice>>    getCaptureDevices() const;
        std::vector<std::shared_ptr<AudioPlaybackDevice>>   getPlaybackDevices() const;        
        std::vector<std::shared_ptr<AbstractAudioDevice>>   getAllDevices() const;
        std::shared_ptr<AudioCaptureDevice>                 getDefaultCaptureDevice();
        std::shared_ptr<AudioPlaybackDevice>                getDefaultPlaybackDevice();
        std::shared_ptr<AudioCaptureDevice>                 getDefaultPlaybackDeviceAsCaptureDevice();
        //std::shared_ptr<AudioCaptureDevice>                 getPlaybackDeviceAsCaptureDevice(std::shared_ptr<AudioPlaybackDevice>&) const;

	private:

        template <typename DeviceType, typename DeviceTypeImpl, ::EDataFlow flowType>
        void enumerateDevicesOfType();

        template <typename DeviceType>
        void removeDevicesOfType();

        template <typename DeviceType>
        std::vector<std::shared_ptr<DeviceType>> getDevicesOfType() const;

        template <typename DeviceType, typename DeviceTypeImpl, ::EDataFlow flowType, ::ERole role>
        std::shared_ptr<DeviceType> getDefaultDeviceOfType(int deviceMode = 0);

        void removeDevice(std::shared_ptr<AbstractAudioDevice> device);

        bool m_initialized;
        std::set<std::shared_ptr<AbstractAudioDevice>> m_devices;
    };

#endif

}
}
