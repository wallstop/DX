
#pragma once

#include "../AbstractAudioDevice.h"
#include "../Tasks/TaskCallback.h"

namespace DX {
namespace Audio {

	class AbstractAudioTask
	{
	public:

		AbstractAudioTask() : m_device(nullptr) 
		{
		}

		AbstractAudioTask(AbstractAudioDevice* device) : m_device(device)
		{
		}

		virtual ~AbstractAudioTask()
		{
		}

		virtual void run(TaskCallback *callback = nullptr) = 0;

	protected:

		::std::shared_ptr<AbstractAudioDevice> m_device;
	};

}
}