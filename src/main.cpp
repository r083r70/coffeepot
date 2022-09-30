
#include "core/app.h"

#if CP_WINDOWS && NDEBUG
#include <windows.h >
#endif

int main(void)
{
// #if CP_WINDOWS && NDEBUG
// 	ShowWindow(GetConsoleWindow(), SW_HIDE);
// #endif

    coffeepot::App app;
    if (app.init())
        app.run();
    
    return 0;
}
