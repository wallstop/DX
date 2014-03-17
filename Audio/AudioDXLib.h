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

// All-in-one include header

#include <AudioDX/AudioCaptureDevice.h>
#include <AudioDX/AudioPlaybackDevice.h>
#include <AudioDX/AudioDeviceManager.h>
#include <AudioDX/AudioFormat.h>
#include <AudioDX/AudioPacket.h>
#include <AudioDX/Filters/AbstractFilter.h>
#include <AudioDX/Tasks/AbstractAudioTask.h>
#include <AudioDX/Tasks/TaskCallback.h>
#include <AudioDX/AudioStream.h>
