
#include "core/app.h"

int main(void)
{
    coffeepot::App app;
    if (app.init())
        app.run();
    return 0;
}