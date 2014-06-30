
#include <Audio/AudioDXLib.h>

using namespace DX::Audio;

int main(int argc, char* argv[])
{
    {
        AudioDeviceManager manager;
        manager.initialize();

        auto playbackDevice = manager.getDefaultPlaybackDevice();

        //playbackDevice->





        auto captureDevice = manager.getDefaultCaptureDevice();

    }

    return 0;
}