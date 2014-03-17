
#pragma once

#include <AudioDX/Tasks/AbstractAudioTask.h>

namespace AudioDX
{

    class AudioSilenceTask : public AbstractAudioTask
    {
    public:

        AudioSilenceTask();
        ~AudioSilenceTask();

        void run(TaskCallback *callback = nullptr);

    private:

        // TODO: Flesh out rest of class

    };

}