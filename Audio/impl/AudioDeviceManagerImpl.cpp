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

#include "../AbstractAudioDevice.h"
#include "../AudioCaptureDevice.h"
#include "../AudioPlaybackDevice.h"

#include "AudioDeviceManagerImpl.h"
#include "AudioCaptureDeviceImpl.h"
#include "AudioPlaybackDeviceImpl.h"

#ifdef WIN32

#include <atlbase.h>

#endif

namespace DX {
namespace Audio {

#ifdef WIN32

    // ONLY CALL IF CoInitialize() has already been called
    // This will clean up after itself on application exit
    static IMMDeviceEnumerator* getDeviceEnumeratorSingleton()
    {
        static IMMDeviceEnumerator* enumerator = nullptr;
        if(!enumerator)
        {
            int ok = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void** >(&enumerator));
            if(ok < 0)
            {
                releaseDevice(enumerator);
            }
        }
        return enumerator;
    }

    AudioDeviceManagerImpl::AudioDeviceManagerImpl() : m_initialized(false)
    {
        initialize();
    }

    AudioDeviceManagerImpl::~AudioDeviceManagerImpl()
    {
        unInitialize();
    }


    bool AudioDeviceManagerImpl::initialize()
    {
        if(!m_initialized)
        {
            int result = 0;
            result = CoInitialize(nullptr);
            m_initialized = result >= 0;

            //enumerateAllDevices();
        }
		return m_initialized;
	}

    bool AudioDeviceManagerImpl::unInitialize()
    {
        if(m_initialized)
        {
            m_devices.clear();
            CoUninitialize();
            m_initialized = false;
        }
        return true;
    }

    bool AudioDeviceManagerImpl::isInitialized() const
    {
        return m_initialized;
    }

    // TODO: Fix
    template <typename DeviceType, typename DeviceTypeImpl, ::EDataFlow flowType>
    void AudioDeviceManagerImpl::enumerateDevicesOfType()
    {
        IMMDeviceEnumerator* deviceEnumerator = getDeviceEnumeratorSingleton();
        if(!deviceEnumerator)
        {
            // Unable to get a handle on the device enumerator
            return;
        }
        
        if(flowType != eRender && flowType != eCapture)
        {
            // Currently we only want to deal with either render OR capture devices, not everything
            return;
        }

        IMMDeviceCollection* deviceCollection = nullptr;
        int ok = deviceEnumerator->EnumAudioEndpoints(flowType, DEVICE_STATE_ACTIVE, &deviceCollection);
        releaseDevice(deviceEnumerator);
        if(ok < 0 || !deviceCollection)
        {
            releaseDevice(deviceCollection);
            return;
        }

        // Clear out everything we know about - we're starting fresh
        removeDevicesOfType<DeviceType>();
        
        unsigned int totalDevices = 0;
        ok = deviceCollection->GetCount(&totalDevices);
        if(ok < 0)
        {
            releaseDevice(deviceCollection);
            return;
        }
        for(unsigned int i = 0; i < totalDevices; ++i)
        {
            IMMDevice* currentDevice = nullptr;
            ok = deviceCollection->Item(i, &currentDevice);
            if(ok < 0 || !currentDevice)
            {
                releaseDevice(currentDevice);
                continue;
            }
            std::shared_ptr<DeviceTypeImpl> deviceImpl = std::make_shared<DeviceTypeImpl>(currentDevice);
            std::shared_ptr<DeviceType> device   = std::make_shared<DeviceType>();
            device->m_impl = deviceImpl;
            m_devices.insert(device);
        }
        releaseDevice(deviceCollection);
    }

    void AudioDeviceManagerImpl::enumeratePlaybackDevices()
    {
        enumerateDevicesOfType<AudioPlaybackDevice, AudioPlaybackDeviceImpl, eRender>();
    }

    void AudioDeviceManagerImpl::enumerateCaptureDevices()
    {
        enumerateDevicesOfType<AudioCaptureDevice, AudioCaptureDeviceImpl, eCapture>();
    }

    void AudioDeviceManagerImpl::enumerateAllDevices()
    {
        m_devices.clear();
        enumeratePlaybackDevices();
        enumerateCaptureDevices();  
    }

    std::vector<std::shared_ptr<AudioPlaybackDevice>> AudioDeviceManagerImpl::getPlaybackDevices() const
    {
        return getDevicesOfType<AudioPlaybackDevice>();
    }

    std::vector<std::shared_ptr<AudioCaptureDevice>> AudioDeviceManagerImpl::getCaptureDevices() const
    {
        return getDevicesOfType<AudioCaptureDevice>();
    }

    std::vector<std::shared_ptr<AbstractAudioDevice>> AudioDeviceManagerImpl::getAllDevices() const
    {
        return std::vector<std::shared_ptr<AbstractAudioDevice>>(m_devices.cbegin(), m_devices.cend());
    }

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManagerImpl::getDefaultCaptureDevice() const
    {
        return getDefaultDeviceOfType<AudioCaptureDevice, AudioCaptureDeviceImpl, eCapture, eConsole>(0);
    }

    std::shared_ptr<AudioPlaybackDevice> AudioDeviceManagerImpl::getDefaultPlaybackDevice() const
    {
        return getDefaultDeviceOfType<AudioPlaybackDevice, AudioPlaybackDeviceImpl, eRender, eConsole>(0);
    }

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManagerImpl::getDefaultPlaybackDeviceAsCaptureDevice() const
    {
        return getDefaultDeviceOfType<AudioCaptureDevice, AudioCaptureDeviceImpl, eRender, eConsole>(AUDCLNT_STREAMFLAGS_LOOPBACK);
    }

    template <typename DeviceType>
    void AudioDeviceManagerImpl::removeDevicesOfType()
    {
        bool finished = false;

        while(!finished)
        {
            // One at a time... there's got to be a better way
            for(auto& device : m_devices)
            {
                if(std::dynamic_pointer_cast<DeviceType>(device))
                {
                    m_devices.erase(device);
                    break;
                }                
            }
            finished = true;
        }

    }

    template <class DeviceType>
    std::vector<std::shared_ptr<DeviceType>> AudioDeviceManagerImpl::getDevicesOfType() const
    {
        std::vector<std::shared_ptr<DeviceType>> ret;
        for(auto& device : m_devices)
        {
            std::shared_ptr<DeviceType> retDevice = std::dynamic_pointer_cast<DeviceType>(device);
            if(retDevice)
                ret.push_back(retDevice);
        }
        return ret;
    }

    template <typename DeviceType, typename DeviceTypeImpl, ::EDataFlow flowType, ::ERole role>
    std::shared_ptr<DeviceType> AudioDeviceManagerImpl::getDefaultDeviceOfType(int deviceMode) const
    {
        if(flowType != eRender && flowType != eCapture)
            return std::shared_ptr<DeviceType>();

        IMMDeviceEnumerator* deviceEnumerator = getDeviceEnumeratorSingleton();
        if(!deviceEnumerator)
            return std::shared_ptr<DeviceType>();

        IMMDevice *device = nullptr;
        int ok = deviceEnumerator->GetDefaultAudioEndpoint(flowType, role, &device);
        releaseDevice(deviceEnumerator);
        if(ok < 0 || !device)
        {
            // Couldn't get the default playback device for some reason
            releaseDevice(device);
            return std::shared_ptr<DeviceType>();
        }

        LPWSTR deviceIdAsLPWSTR;
        device->GetId(&deviceIdAsLPWSTR);
        const std::string deviceId = CW2A(deviceIdAsLPWSTR);

        // So now we check if we have a device with a handle to that particular IMMDevice
        for(auto& device : m_devices)
        {
            if(device.get() != nullptr && device->m_impl != nullptr && device->m_impl->m_mmDevice != nullptr)
            {
                if(device->id().compare(deviceId) == 0)
                {
                    // Make sure we're not returning a handle to the wrong kind of device
                    auto ret = std::dynamic_pointer_cast<DeviceType>(device);
                    if(ret && device->m_impl->m_deviceMode == deviceMode)
                        return ret;
                }
            }
        }

        // If we haven't found it in our set (why isn't it there? That's weird), let's make it
        std::shared_ptr<DeviceTypeImpl> defaultDeviceImpl = std::make_shared<DeviceTypeImpl>(device);
        std::shared_ptr<DeviceType> defaultDevice = std::make_shared<DeviceType>();
        defaultDeviceImpl->m_deviceMode = deviceMode;
        defaultDevice->m_impl = defaultDeviceImpl;        
    
        return defaultDevice;
    }

#endif

}
}