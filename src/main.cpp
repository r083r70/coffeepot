
#include "core/app.h"
#include "core/platform.h"

#if WITH_NOTIFYICON
#include <windows.h>
#endif

#if WITH_NOTIFYICON
int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
#elif CP_WINDOWS && NDEBUG
int WinMain()
#else
int main()
#endif
{
    coffeepot::BasePlatform* platform = IF_ELSE_NOTIFYICON(
        new coffeepot::WindowsPlatform(instance),
        new coffeepot::BasePlatform()
    );

    coffeepot::App app{ platform };
    if (app.init())
        app.run();
    
    return 0;
}