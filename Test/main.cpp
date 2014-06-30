
#include <Audio/AudioDXLib.h>

using namespace DX::Audio;

int main(int argc, char* argv[])
{
    {
        AudioDeviceManager manager;
        manager.initialize();

        auto playbackDevice = manager.getDefaultPlaybackDevice();
        playbackDevice->initialize();

        //playbackDevice->

        auto captureDevice = manager.getDefaultCaptureDevice();
        captureDevice->initialize();

    }

    return 0;
}