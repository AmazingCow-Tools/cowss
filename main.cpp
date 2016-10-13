
//std
#include <cstring>
//SS
#include "SS.h"


int main(int argc, const char *argv[])
{
    auto ssType = SS::ScreenShotType::Fullscreen;
    if(argc > 1 && strcmp(argv[1], "grab") == 0)
        auto ssType = SS::ScreenShotType::Selection;

    auto ss = SS(ssType);

    ss.loop();
    ss.saveScreenshot();

    return EXIT_SUCCESS;
}

