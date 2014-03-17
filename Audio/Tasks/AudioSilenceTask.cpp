
#include <AudioDX/Tasks/AudioSilenceTask.h>

namespace AudioDX
{

    AudioSilenceTask::AudioSilenceTask()
    {
    }

    AudioSilenceTask::~AudioSilenceTask()
    {
    }

    void AudioSilenceTask::run(TaskCallback *callback)
    {
        if(!m_device)
            return;

        // TODO: STUB
    }

}