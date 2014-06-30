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
#include <functiondiscoverykeys_devpkey.h>

#endif

namespace DX {
namespace Audio {

#ifdef WIN32

    // ONLY CALL IF CoInitialize() has already been called
    // This will clean up after itself on application exit (poorly)
    static IMMDeviceEnumerator*& getDeviceEnumeratorSingleton()
    {
        static IMMDeviceEnumerator* enumerator = nullptr;
        if(!enumerator)
        {
            int ok = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void** >(&enumerator));
            if(ok < 0)
                releaseDevice(enumerator);
        }
        return enumerator;
    }

    static std::string getDeviceID(IMMDevice* mmDevice)
    {
        // Try and grab a friendly version of the device name
        std::string ret = "";
        IPropertyStore *propertyStore = nullptr;
        int ok = mmDevice->OpenPropertyStore(STGM_READ, &propertyStore);
        if(ok < 0 || propertyStore == nullptr)
        {
            // Unable to get the property store (that's ok, just continue)
            releaseDevice(propertyStore);
        }
        else
        {
            PROPVARIANT propVariant;
            PropVariantInit(&propVariant);
            ok = propertyStore->GetValue(PKEY_Device_FriendlyName, &propVariant);
            releaseDevice(propertyStore);
            if(ok >= 0 && VT_LPWSTR == propVariant.vt)
                ret = CW2A(propVariant.pwszVal);
        }

        return ret;
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
            const int result = CoInitialize(nullptr);
            m_initialized = result >= 0;
            if(m_initialized)
            {
                getDeviceEnumeratorSingleton();
                enumerateAllDevices();
            }
        }
		return m_initialized;
	}

    bool AudioDeviceManagerImpl::unInitialize()
    {
        if(m_initialized)
        {
            m_devices.clear();
            //auto deviceEnumerator = getDeviceEnumeratorSingleton();
            //releaseDevice(deviceEnumerator);
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
        IMMDeviceEnumerator*& deviceEnumerator = getDeviceEnumeratorSingleton();
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
            // TODO: Change this from 0 to some parameterized type or something
            std::shared_ptr<DeviceTypeImpl> deviceImpl = std::make_shared<DeviceTypeImpl>(currentDevice);
            std::shared_ptr<DeviceType> device = std::make_shared<DeviceType>();
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

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManagerImpl::getDefaultCaptureDevice()
    {
        return getDefaultDeviceOfType<AudioCaptureDevice, AudioCaptureDeviceImpl, eCapture, eConsole>(0);
    }

    std::shared_ptr<AudioPlaybackDevice> AudioDeviceManagerImpl::getDefaultPlaybackDevice()
    {
        return getDefaultDeviceOfType<AudioPlaybackDevice, AudioPlaybackDeviceImpl, eRender, eConsole>(0);
    }

    std::shared_ptr<AudioCaptureDevice> AudioDeviceManagerImpl::getDefaultPlaybackDeviceAsCaptureDevice()
    {
        return getDefaultDeviceOfType<AudioCaptureDevice, AudioCaptureDeviceImpl, eRender, eConsole>(AUDCLNT_STREAMFLAGS_LOOPBACK);
    }

    template <typename DeviceType>
    void AudioDeviceManagerImpl::removeDevicesOfType()
    {
        auto iterator = m_devices.begin();
        while(iterator != m_devices.end())
        {
            auto current = iterator++;
            if(std::dynamic_pointer_cast<DeviceType>(*current))
                m_devices.erase(*current);
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

    /*
        TODO: This is an extremely bad way of performing this operation, as we most likely already have the device
        in our m_devices set. Blowing away the devices is bad, making a new handle is bad (as the destruction of that
        object will screw everything up for the object in the set, although they won't know)

        So, figure out a way to fix this.
    */
    template <typename DeviceType, typename DeviceTypeImpl, ::EDataFlow flowType, ::ERole role>
    std::shared_ptr<DeviceType> AudioDeviceManagerImpl::getDefaultDeviceOfType(int deviceMode)
    {
        if(flowType != eRender && flowType != eCapture)
            return std::shared_ptr<DeviceType>();

        IMMDeviceEnumerator*& deviceEnumerator = getDeviceEnumeratorSingleton();
        if(!deviceEnumerator)
            return std::shared_ptr<DeviceType>();

        IMMDevice *device = nullptr; 
        auto ptr = &device;
        int ok = deviceEnumerator->GetDefaultAudioEndpoint(flowType, role, ptr);
        releaseDevice(deviceEnumerator);
        if(ok < 0 || !device)
        {
            // Couldn't get the default playback device for some reason
            releaseDevice(device);
            return std::shared_ptr<DeviceType>();
        }

        const std::string deviceId = getDeviceID(device);
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

        // TODO: Currently this is bad and will not properly blow things away if someone has a handle to our device with a different device mode...
        std::shared_ptr<DeviceTypeImpl> defaultDeviceImpl = std::make_shared<DeviceTypeImpl>(DeviceTypeImpl(device, nullptr, deviceMode));
        std::shared_ptr<DeviceType> defaultDevice = std::make_shared<DeviceType>();
        defaultDevice->m_impl = defaultDeviceImpl;   
        m_devices.insert(defaultDevice);    

        return defaultDevice;
    }

#endif

}
}