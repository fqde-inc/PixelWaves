#include "PixelWavesApplication.h"
#include <Windows.h>

int main()
{
    PixelWavesApplication sceneViewerApplication;

    //mciSendString("play music/windwaker.mp3 repeat", NULL, 0, NULL);

    return sceneViewerApplication.Run();
}
